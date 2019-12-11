#include <thread>

#include "../utils.hpp"
#include "intcode.hpp"

namespace intcode {

Parameter::Parameter(Value value, Value new_mode) : 
	value(value),
	mode(static_cast<Parameter::Mode>(new_mode)) {}

CPU::CPU(Memory& memory, Data& input, Data& output, Value base) :
	memory(memory), input(input), output(output), base(base) {}

Instruction::Instruction(Pointer& pointer, Parameters parameters)
	: m_pointer(pointer), m_parameters(parameters) {}

Value Instruction::get_parameter_with_mode(CPU& cpu, int parameter_id) {
	auto& param = m_parameters[parameter_id];
	if (param.mode == Parameter::Mode::IMMEDIATE) {
		return param.value;
	} else {
		return cpu.memory[param.value];
	}
}

void Instruction::store_to_parameter(CPU& cpu, int parameter_id, int64_t value) {
	cpu.memory[m_parameters[parameter_id].value] = value;
}

Add::Add(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void Add::execute(CPU& cpu) {
	execute_instruction(cpu, [](auto src1, auto src2) { return src1 + src2; });
}

std::unique_ptr<Add> Add::from_source(Pointer& source, Value modes) {
	return Instruction::from_source<Add>(source, modes, 3);
}

Multiply::Multiply(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void Multiply::execute(CPU& cpu) {
	execute_instruction(cpu, [](auto src1, auto src2) { return src1 * src2; });
}

std::unique_ptr<Multiply> Multiply::from_source(Pointer& source, Value modes) {
	return Instruction::from_source<Multiply>(source, modes, 3);
}

Input::Input(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void Input::execute(CPU& cpu) {
	store_to_parameter(cpu, 0, cpu.input.front());
	cpu.input.pop_front();
}

std::unique_ptr<Input> Input::from_source(Pointer& source, Value modes) {
	return Instruction::from_source<Input>(source, modes, 1);
}

Output::Output(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void Output::execute(CPU& cpu) {
	cpu.output.push_back(get_parameter_with_mode(cpu, 0));
}

std::unique_ptr<Output> Output::from_source(Pointer& source, Value modes) {
	return Instruction::from_source<Output>(source, modes, 1);
}

JumpTrue::JumpTrue(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void JumpTrue::execute(CPU& cpu) {
	auto first = get_parameter_with_mode(cpu, 0);
	if (first != 0) {
		m_pointer = cpu.memory.begin() + get_parameter_with_mode(cpu, 1);
	}
}

std::unique_ptr<JumpTrue> JumpTrue::from_source(Pointer& source, Value modes) {
	return Instruction::from_source<JumpTrue>(source, modes, 2);
}

JumpFalse::JumpFalse(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void JumpFalse::execute(CPU& cpu) {
	auto first = get_parameter_with_mode(cpu, 0);
	if (first == 0) {
		m_pointer = cpu.memory.begin() + get_parameter_with_mode(cpu, 1);
	}
}

std::unique_ptr<JumpFalse> JumpFalse::from_source(Pointer& source, Value modes) {
	return Instruction::from_source<JumpFalse>(source, modes, 2);
}

LessThan::LessThan(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void LessThan::execute(CPU& cpu) {
	auto first = get_parameter_with_mode(cpu, 0);
	auto second = get_parameter_with_mode(cpu, 1);
	int to_store = 0;
	if (first < second) {
		to_store = 1;
	}
	store_to_parameter(cpu, 2, to_store);
}

std::unique_ptr<LessThan> LessThan::from_source(Pointer& source, Value modes) {
	return Instruction::from_source<LessThan>(source, modes, 3);
}

Equal::Equal(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void Equal::execute(CPU& cpu) {
	Value to_store = 0;
	if (get_parameter_with_mode(cpu, 0) == get_parameter_with_mode(cpu, 1)) {
		to_store = 1;
	}
	store_to_parameter(cpu, 2, to_store);
}

std::unique_ptr<Equal> Equal::from_source(Pointer& source, Value modes) {
	return Instruction::from_source<Equal>(source, modes, 3);
}

Stop::Stop(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void Stop::execute(CPU& /*cpu*/) { }

std::unique_ptr<Stop> Stop::from_source(Pointer& source, Value modes) {
	return Instruction::from_source<Stop>(source, modes, 0);
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

void run_program_on_computer_with_id(Program& program, Memory::size_type id, Hooks instruction_hooks) {
	std::unique_ptr<Instruction> inst;
	auto& comp = program.at(id);
	auto iter = comp.cpu.memory.begin();
	while (iter != comp.cpu.memory.end()) {
		auto opcode = static_cast<Type>(*iter % 100);
		auto modes = *iter / 100;
		iter++;
		switch (opcode) {
			case Type::ADD:
				inst = Add::from_source(iter, modes);
				break;
			case Type::MULTIPLY:
				inst = Multiply::from_source(iter, modes);
				break;
			case Type::INPUT:
				inst = Input::from_source(iter, modes);
				break;
			case Type::OUTPUT:
				inst = Output::from_source(iter, modes);
				break;
			case Type::JNZ:
				inst = JumpTrue::from_source(iter, modes);
				break;
			case Type::JZ:
				inst = JumpFalse::from_source(iter, modes);
				break;
			case Type::LT:
				inst = LessThan::from_source(iter, modes);
				break;
			case Type::EQ:
				inst = Equal::from_source(iter, modes);
				break;
			case Type::STOP:
				return;
			default:
				break;
		}
		if (instruction_hooks.contains(opcode)) {
			instruction_hooks[opcode](program, comp, inst);
		} else {
			inst->execute(comp.cpu);
		}
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
