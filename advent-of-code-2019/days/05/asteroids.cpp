
#include <iostream>

#include "../day_factory.hpp"
#include "asteroids.hpp"

std::string Asteroids::part_01() {
    run_program_from_source(src);
    return std::string();
}

std::string Asteroids::part_02() {
    // Is solved by part 1 by giving input: 5
    return std::string();
}

std::unique_ptr<Day> Asteroids::create() {
    return std::make_unique<Asteroids>();
}

std::string Asteroids::name() {
    return "day05";
}

void Asteroids::run_program_from_source(const std::string& source) {
    parse_source_to_memory(source);
    auto iter = memory.begin();
    while (iter != memory.end()) {
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
                return;
            default:
                break;
        }
        inst->execute(memory);
    }
}

void Asteroids::parse_source_to_memory(const std::string& source) {
    memory.clear();
    for (const auto& elem : split_string_by(source, ",")) {
        memory.push_back(std::stoi(elem));
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

void Add::execute(Memory& memory) { 
    execute_instruction(memory, [](auto src1, auto src2) { return src1 + src2; });
}

Multiply::Multiply(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void Multiply::execute(Memory& memory) {
    execute_instruction(memory, [](auto src1, auto src2) { return src1 * src2; });
}

Input::Input(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void Input::execute(Memory& memory) {
    int value;
    std::cin >> value;
    memory[m_parameters[0].value] = value;
}

Output::Output(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void Output::execute(Memory& memory) { 
    std::cout << get_parameter_with_mode(memory, 0) << std::endl;
}

JumpTrue::JumpTrue(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void JumpTrue::execute(Memory& memory) {
    auto first = get_parameter_with_mode(memory, 0);
    if (first != 0) {
        m_pointer = memory.begin() + get_parameter_with_mode(memory, 1);
    }
}

JumpFalse::JumpFalse(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void JumpFalse::execute(Memory& memory) {
    auto first = get_parameter_with_mode(memory, 0);
    if (first == 0) {
        m_pointer = memory.begin() + get_parameter_with_mode(memory, 1);
    }
}

LessThan::LessThan(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void LessThan::execute(Memory& memory) {
    auto first = get_parameter_with_mode(memory, 0);
    auto second = get_parameter_with_mode(memory, 1);
    int to_store = 0;
    if (first < second) {
        to_store = 1;
    }
    store_to_parameter(memory, 2, to_store);
}

Equal::Equal(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void Equal::execute(Memory& memory) {
    auto first = get_parameter_with_mode(memory, 0);
    auto second = get_parameter_with_mode(memory, 1);
    int to_store = 0;
    if (first == second) {
        to_store = 1;
    }
    store_to_parameter(memory, 2, to_store);
}

Stop::Stop(Pointer& pointer, Parameters parameters) : Instruction(pointer, parameters) {}

void Stop::execute(Memory& memory) { (void)memory; }

bool Asteroids::s_registered = DayFactory::register_day(Asteroids::name(), Asteroids::create);
