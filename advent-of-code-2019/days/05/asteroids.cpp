
#include <iostream>

#include "../day_factory.hpp"
#include "asteroids.hpp"

std::string Asteroids::part_01() {
    Data input = {1};
    return std::to_string(run_program_from_source(src, input).back());
}

std::string Asteroids::part_02() {
    Data input = {5};
    return std::to_string(run_program_from_source(src, input).back());
}

std::unique_ptr<Day> Asteroids::create() {
    return std::make_unique<Asteroids>();
}

std::string Asteroids::name() {
    return "day05";
}

Data Asteroids::run_program_from_source(const std::string& source, Data& input) {
    Data output;
    parse_source_to_memory(source);
    auto iter = m_memory.begin();
    while (iter != m_memory.end()) {
        auto opcode = static_cast<Type>(*iter % 100);
        auto modes = *iter / 100;
        iter++;
        std::unique_ptr<Instruction> inst;
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
                return output;
            default:
                break;
        }
        inst->execute(m_memory, input, output);
    }
    return output;
}

void Asteroids::parse_source_to_memory(const std::string& source) {
    m_memory.clear();
    for (const auto& elem : split_string_by(source, ",")) {
        m_memory.push_back(std::stoi(elem));
    }
}

Parameter::Parameter(int value, int new_mode) : value(value), mode(Parameter::Mode::POSITION) {
    if (new_mode == 0) {
        mode = Mode::POSITION;
    } else if (new_mode == 1) {
        mode = Mode::IMMEDIATE;
    }
}

Instruction::Instruction(Pointer& pointer, Parameters parameters)
    : m_pointer(pointer), m_parameters(parameters) {}

int Instruction::get_parameter_with_mode(Memory& memory, int parameter_id) {
    auto& param = m_parameters[parameter_id];
    if (param.mode == Parameter::Mode::IMMEDIATE) {
        return param.value;
    } else {
        return memory[param.value];
    }
    // Should not reach.
}

void Instruction::store_to_parameter(Memory& memory, int parameter_id, int value) {
    memory[m_parameters[parameter_id].value] = value;
}

Add::Add(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void Add::execute(Memory& memory, Data& /*input*/, Data& /*output*/) {
    execute_instruction(memory, [](auto src1, auto src2) { return src1 + src2; });
}

Multiply::Multiply(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void Multiply::execute(Memory& memory, Data& /*input*/, Data& /*output*/) {
    execute_instruction(memory, [](auto src1, auto src2) { return src1 * src2; });
}

Input::Input(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void Input::execute(Memory& memory, Data& input, Data& /*output*/) {
    memory[m_parameters[0].value] = input.front();
    input.pop_front();
}

Output::Output(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void Output::execute(Memory& memory, Data& /*input*/, Data& output) {
    output.push_back(get_parameter_with_mode(memory, 0));
}

JumpTrue::JumpTrue(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void JumpTrue::execute(Memory& memory, Data& /*input*/, Data& /*output*/) {
    auto first = get_parameter_with_mode(memory, 0);
    if (first != 0) {
        m_pointer = memory.begin() + get_parameter_with_mode(memory, 1);
    }
}

JumpFalse::JumpFalse(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void JumpFalse::execute(Memory& memory, Data& /*input*/, Data& /*output*/) {
    auto first = get_parameter_with_mode(memory, 0);
    if (first == 0) {
        m_pointer = memory.begin() + get_parameter_with_mode(memory, 1);
    }
}

LessThan::LessThan(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void LessThan::execute(Memory& memory, Data& /*input*/, Data& /*output*/) {
    auto first = get_parameter_with_mode(memory, 0);
    auto second = get_parameter_with_mode(memory, 1);
    int to_store = 0;
    if (first < second) {
        to_store = 1;
    }
    store_to_parameter(memory, 2, to_store);
}

Equal::Equal(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void Equal::execute(Memory& memory, Data& /*input*/, Data& /*output*/) {
    auto first = get_parameter_with_mode(memory, 0);
    auto second = get_parameter_with_mode(memory, 1);
    int to_store = 0;
    if (first == second) {
        to_store = 1;
    }
    store_to_parameter(memory, 2, to_store);
}

Stop::Stop(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void Stop::execute(Memory& /*memory*/, Data& /*input*/, Data& /*output*/) { }

bool Asteroids::s_registered = DayFactory::register_day(Asteroids::name(), Asteroids::create);
