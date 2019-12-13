#include <thread>

#include "../utils.hpp"
#include "intcode.hpp"

namespace intcode {

Parameter::Parameter(Value value, Value new_mode) :
	value(value),
	mode(static_cast<Parameter::Mode>(new_mode)) {}

CPU::CPU(Memory& memory, Data& input, Data& output, Value base) :
	memory(memory), input(input), output(output), base(base) {}

void CPU::memory_size_guard(Value location) {
	if (location >= static_cast<Value>(memory.size())) {
		memory.resize(location+ 1, 0);
	}
}

Instruction::Instruction(CPU& cpu, Parameters parameters, Value ip)
	: ip(ip), parameters(parameters), cpu(cpu) {}

Value Instruction::get_ip() const {
	return ip;
}

Value Instruction::get_parameter_with_mode(CPU& cpu, int parameter_id) {
	auto& param = parameters[parameter_id];
	Value position = 0;
	switch (param.mode) {
	case Parameter::Mode::IMMEDIATE:
		return param.value;
	case Parameter::Mode::POSITION:
		position = param.value;
		break;
	case Parameter::Mode::RELATIVE:
		position = cpu.base + param.value;
		break;
	default:
		break;
	}
	cpu.memory_size_guard(position);
	return cpu.memory.at(position);
}

void Instruction::store_to_parameter(CPU& cpu, int parameter_id, Value value) {
	auto& param = parameters[parameter_id];
	Value position = 0;
	switch (param.mode) {
	case Parameter::Mode::POSITION:
		position = param.value;
		break;
	case Parameter::Mode::RELATIVE:
		position = cpu.base + param.value;
		break;
	default:
		break;
	}
	cpu.memory_size_guard(position);
	cpu.memory[position] = value;
}

Add::Add(CPU& cpu, Parameters parameters, Value ip) :
	Instruction(cpu, parameters, ip) {}

void Add::execute(CPU& cpu) {
	execute_instruction(cpu, [](auto src1, auto src2) { return src1 + src2; });
}

std::unique_ptr<Add> Add::from_source(CPU& cpu, Value modes, Value ip) {
	return Instruction::from_source<Add>(cpu, modes, ip, 3);
}

Multiply::Multiply(CPU& cpu, Parameters parameters, Value ip) :
	Instruction(cpu, parameters, ip) {}

void Multiply::execute(CPU& cpu) {
	execute_instruction(cpu, [](auto src1, auto src2) { return src1 * src2; });
}

std::unique_ptr<Multiply> Multiply::from_source(CPU& cpu, Value modes, Value ip) {
	return Instruction::from_source<Multiply>(cpu, modes, ip, 3);
}

Input::Input(CPU& cpu, Parameters parameters, Value ip) :
	Instruction(cpu, parameters, ip) {}

void Input::execute(CPU& cpu) {
	store_to_parameter(cpu, 0, cpu.input.front());
	cpu.input.pop_front();
}

std::unique_ptr<Input> Input::from_source(CPU& cpu, Value modes, Value ip) {
	return Instruction::from_source<Input>(cpu, modes, ip, 1);
}

Output::Output(CPU& cpu, Parameters parameters, Value ip) :
	Instruction(cpu, parameters, ip) {}

void Output::execute(CPU& cpu) {
	cpu.output.push_back(get_parameter_with_mode(cpu, 0));
}

std::unique_ptr<Output> Output::from_source(CPU& cpu, Value modes, Value ip) {
	return Instruction::from_source<Output>(cpu, modes, ip, 1);
}

JumpTrue::JumpTrue(CPU& cpu, Parameters parameters, Value ip) :
	Instruction(cpu, parameters, ip) {}

void JumpTrue::execute(CPU& cpu) {
	auto first = get_parameter_with_mode(cpu, 0);
	if (first != 0) {
		ip = get_parameter_with_mode(cpu, 1);
	}
}

std::unique_ptr<JumpTrue> JumpTrue::from_source(CPU& cpu, Value modes, Value ip) {
	return Instruction::from_source<JumpTrue>(cpu, modes, ip, 2);
}

JumpFalse::JumpFalse(CPU& cpu, Parameters parameters, Value ip) :
	Instruction(cpu, parameters, ip) {}

void JumpFalse::execute(CPU& cpu) {
	auto first = get_parameter_with_mode(cpu, 0);
	if (first == 0) {
		ip = get_parameter_with_mode(cpu, 1);
	}
}

std::unique_ptr<JumpFalse> JumpFalse::from_source(CPU& cpu, Value modes, Value ip) {
	return Instruction::from_source<JumpFalse>(cpu, modes, ip, 2);
}

LessThan::LessThan(CPU& cpu, Parameters parameters, Value ip) :
	Instruction(cpu, parameters, ip) {}

void LessThan::execute(CPU& cpu) {
	auto first = get_parameter_with_mode(cpu, 0);
	auto second = get_parameter_with_mode(cpu, 1);
	int to_store = 0;
	if (first < second) {
		to_store = 1;
	}
	store_to_parameter(cpu, 2, to_store);
}

std::unique_ptr<LessThan> LessThan::from_source(CPU& cpu, Value modes, Value ip) {
	return Instruction::from_source<LessThan>(cpu, modes, ip, 3);
}

Equal::Equal(CPU& cpu, Parameters parameters, Value ip) :
	Instruction(cpu, parameters, ip) {}

void Equal::execute(CPU& cpu) {
	Value to_store = 0;
	if (get_parameter_with_mode(cpu, 0) == get_parameter_with_mode(cpu, 1)) {
		to_store = 1;
	}
	store_to_parameter(cpu, 2, to_store);
}

std::unique_ptr<Equal> Equal::from_source(CPU& cpu, Value modes, Value ip) {
	return Instruction::from_source<Equal>(cpu, modes, ip, 3);
}

Base::Base(CPU& cpu, Parameters parameters, Value ip) :
	Instruction(cpu, parameters, ip) {}

void Base::execute(CPU& cpu) {
	cpu.base += get_parameter_with_mode(cpu, 0);
}

std::unique_ptr<Base> Base::from_source(CPU& cpu, Value modes, Value ip) {
	return Instruction::from_source<Base>(cpu, modes, ip, 1);
}

Stop::Stop(CPU& cpu, Parameters parameters, Value ip) :
	Instruction(cpu, parameters, ip) {}

void Stop::execute(CPU& /*cpu*/) { }

std::unique_ptr<Stop> Stop::from_source(CPU& cpu, Value modes, Value ip) {
	return Instruction::from_source<Stop>(cpu, modes, ip, 0);
}

Computer::Computer(Memory n_memory, Data initial_input, Memory::size_type n_feed_to) :
	memory(n_memory),
	input(initial_input),
	feed_to(n_feed_to),
	cpu(memory, input, output, 0) {}

void Computer::add_to_input(Value value) {
	std::lock_guard lock(m);
	input.push_back(value);
	cv.notify_one();
}

void Computer::wait_for_input() {
	std::unique_lock lock(m);
	cv.wait(lock, [&] { return input.size() != 0;});
	lock.unlock();
}

Value Computer::wait_for_input_and_pop() {
	std::unique_lock lock(m);
	cv.wait(lock, [&] {return input.size() != 0;});
	auto value = input.front();
	input.pop_front();
	lock.unlock();
	return value;
}

std::unique_ptr<Instruction> instruction_factory(CPU& cpu, Value ip) {
	auto first_operand = cpu.memory.at(ip++);
	auto opcode = static_cast<Type>(first_operand % 100);
	auto modes = first_operand / 100;
	switch (opcode) {
	case Type::ADD:
		return Add::from_source(cpu, modes, ip);
	case Type::MULTIPLY:
		return Multiply::from_source(cpu, modes, ip);
	case Type::INPUT:
		return Input::from_source(cpu, modes, ip);
	case Type::OUTPUT:
		return Output::from_source(cpu, modes, ip);
	case Type::JNZ:
		return JumpTrue::from_source(cpu, modes, ip);
	case Type::JZ:
		return JumpFalse::from_source(cpu, modes, ip);
	case Type::LT:
		return LessThan::from_source(cpu, modes, ip);
	case Type::BASE:
		return Base::from_source(cpu, modes, ip);
	case Type::EQ:
		return Equal::from_source(cpu, modes, ip);
	case Type::STOP:
		return Stop::from_source(cpu, modes, ip);
	default:
		break;
	}
	return std::unique_ptr<Instruction>(nullptr);
}

void run_program_on_computer_with_id(Program& program, Memory::size_type id, Hooks instruction_hooks) {
	auto& comp = program.at(id);
	Value ip = 0;
	for (;;) {
		auto inst = instruction_factory(comp.cpu, ip);
		auto opcode = inst->get_opcode();
		if (instruction_hooks.contains(opcode)) {
			instruction_hooks[opcode](program, comp, inst);
		} else {
			inst->execute(comp.cpu);
		}
		if (opcode == Type::STOP) {
			return;
		}
		ip = inst->get_ip();
	}
	return; // Should not be reached.
}

void run_program_with_phase_settings(Program& program, Memory phase_settings, Hooks instruction_hooks) {
	if (program.size() != phase_settings.size()) {
		return;
	}
	std::vector<std::thread> threads(program.size());
	for (Program::size_type i = 0; i < program.size(); i++) {
		threads[i] = std::thread(
			run_program_on_computer_with_id,
			std::ref(program),
			phase_settings[i],
			instruction_hooks
		);
	}
	for (Program::size_type i = 0; i < program.size(); i++) {
		threads[i].join();
	}
}

Program get_program_for_memory_with_phase_settings(const Memory& memory, const Memory& phase_settings) {
	Program program;
	for (Memory::size_type i = 0; i < phase_settings.size(); i++) {
		auto id = phase_settings[i];
		auto idx = (i + 1) % phase_settings.size();
		auto feed_to = phase_settings[idx];
		Data initial_input = {id};
		if (i == 0) {
			initial_input.push_back(0);
		}
		program.emplace(
			std::piecewise_construct,
			std::make_tuple(id),
			std::make_tuple(memory, initial_input, feed_to)
		);
	}
	return program;
}

Program get_program_for_memory_with_patched_data(const Memory& memory, const Memory& patch) {
	Memory mem = memory;
	mem[1] = patch[0];
	mem[2] = patch[1];
	return get_program_for_memory_with_input_data(mem, Data());
}

Program get_program_for_memory_with_input_data(const Memory& memory, const Data& data) {
	Program program;
	program.emplace(
		std::piecewise_construct,
		std::make_tuple(0),
		std::make_tuple(memory, data, 0)
	);
	return program;
}

Memory get_memory_from_string(const std::string& source) {
	Memory memory;
	auto partially_parsed = split_string_by(source, ",");
	for (const auto& elem : partially_parsed) {
		memory.push_back(std::stoll(elem));
	}
	return memory;
}

}
