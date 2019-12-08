
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "../day_factory.hpp"
#include "asteroids.hpp"

void Asteroids::part_01() {
    std::string src = "3,225,1,225,6,6,1100,1,238,225,104,0,1101,69,55,225,1001,144,76,224,101,-139,224,224,4,224,1002,223,8,223,1001,224,3,224,1,223,224,223,1102,60,49,225,1102,51,78,225,1101,82,33,224,1001,224,-115,224,4,224,1002,223,8,223,1001,224,3,224,1,224,223,223,1102,69,5,225,2,39,13,224,1001,224,-4140,224,4,224,102,8,223,223,101,2,224,224,1,224,223,223,101,42,44,224,101,-120,224,224,4,224,102,8,223,223,101,3,224,224,1,223,224,223,1102,68,49,224,101,-3332,224,224,4,224,1002,223,8,223,1001,224,4,224,1,224,223,223,1101,50,27,225,1102,5,63,225,1002,139,75,224,1001,224,-3750,224,4,224,1002,223,8,223,1001,224,3,224,1,223,224,223,102,79,213,224,1001,224,-2844,224,4,224,102,8,223,223,1001,224,4,224,1,223,224,223,1,217,69,224,1001,224,-95,224,4,224,102,8,223,223,1001,224,5,224,1,223,224,223,1102,36,37,225,1101,26,16,225,4,223,99,0,0,0,677,0,0,0,0,0,0,0,0,0,0,0,1105,0,99999,1105,227,247,1105,1,99999,1005,227,99999,1005,0,256,1105,1,99999,1106,227,99999,1106,0,265,1105,1,99999,1006,0,99999,1006,227,274,1105,1,99999,1105,1,280,1105,1,99999,1,225,225,225,1101,294,0,0,105,1,0,1105,1,99999,1106,0,300,1105,1,99999,1,225,225,225,1101,314,0,0,106,0,0,1105,1,99999,1107,677,677,224,102,2,223,223,1006,224,329,1001,223,1,223,1108,677,677,224,1002,223,2,223,1006,224,344,1001,223,1,223,107,226,226,224,1002,223,2,223,1006,224,359,101,1,223,223,1008,226,226,224,102,2,223,223,1005,224,374,1001,223,1,223,1107,226,677,224,1002,223,2,223,1006,224,389,1001,223,1,223,1008,677,226,224,1002,223,2,223,1005,224,404,1001,223,1,223,7,677,226,224,102,2,223,223,1005,224,419,1001,223,1,223,1008,677,677,224,1002,223,2,223,1006,224,434,1001,223,1,223,108,226,226,224,102,2,223,223,1006,224,449,1001,223,1,223,108,677,677,224,102,2,223,223,1006,224,464,1001,223,1,223,107,226,677,224,1002,223,2,223,1005,224,479,101,1,223,223,1108,226,677,224,1002,223,2,223,1006,224,494,1001,223,1,223,107,677,677,224,1002,223,2,223,1006,224,509,101,1,223,223,7,677,677,224,102,2,223,223,1006,224,524,1001,223,1,223,1007,226,677,224,1002,223,2,223,1005,224,539,1001,223,1,223,8,226,677,224,1002,223,2,223,1005,224,554,101,1,223,223,8,677,677,224,102,2,223,223,1005,224,569,101,1,223,223,7,226,677,224,102,2,223,223,1006,224,584,1001,223,1,223,1007,226,226,224,102,2,223,223,1006,224,599,1001,223,1,223,1107,677,226,224,1002,223,2,223,1006,224,614,1001,223,1,223,1108,677,226,224,1002,223,2,223,1005,224,629,1001,223,1,223,1007,677,677,224,102,2,223,223,1006,224,644,1001,223,1,223,108,226,677,224,102,2,223,223,1005,224,659,101,1,223,223,8,677,226,224,1002,223,2,223,1006,224,674,1001,223,1,223,4,223,99,226";
    run_program_from_source(src);
}

void Asteroids::part_02() {
    // Is solved by part 1 by giving input: 5
}

std::unique_ptr<Day> Asteroids::create() {
    return std::make_unique<Asteroids>();
}

std::string Asteroids::name() {
    return "day05";
}

void Asteroids::run_program_from_source(std::string_view source) {
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

void Asteroids::parse_source_to_memory(std::string_view source) {
    memory.clear();
    std::vector<std::string> partially_parsed;
    boost::split(partially_parsed, source, boost::is_any_of(","));
    for (const auto& elem : partially_parsed) {
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
