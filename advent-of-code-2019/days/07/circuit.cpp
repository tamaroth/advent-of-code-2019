#include <iostream>

#include "circuit.hpp"
#include "../day_factory.hpp"


Amplifier::Amplifier(Memory memory, Data initial_input, int feed_to) :
    memory(memory),
    input(initial_input),
    feed_to(feed_to) {}

void Amplifier::add_to_input(int value) {
    std::lock_guard lock(m);
    input.push_back(value);
    cv.notify_one();
}

void Amplifier::wait_for_input() {
    std::unique_lock lock(m);
    cv.wait(lock, [&] { return input.size() != 0;});
    lock.unlock();
}

std::string Circuit::part_01() {
    Memory phase_setting = {0, 1, 2, 3, 4};
    int max_thruster_signal = 0;
    do
    {
        max_thruster_signal = std::max(
            max_thruster_signal,
            get_thruster_signal_for_phase_setting_sequence(phase_setting)
        );
    } while (std::next_permutation(phase_setting.begin(), phase_setting.end()));
    return std::to_string(max_thruster_signal);
}

std::string Circuit::part_02() {
    auto memory = get_memory_from_source(src);
    Memory phase_settings = {5, 6, 7, 8, 9};
    int max_thruster_signal = 0;
    do {
        max_thruster_signal = std::max(
            max_thruster_signal,
            run_program_with_phase_settings(memory, phase_settings)
        );
    } while (std::next_permutation(phase_settings.begin(), phase_settings.end()));
    return std::to_string(max_thruster_signal);
}

std::unique_ptr<Day> Circuit::create() {
    return std::make_unique<Circuit>();
}

std::string Circuit::name() {
    return "day07";
}

int Circuit::get_thruster_signal_for_phase_setting_sequence(const Memory& phase_setting) {
    int max_thruster_signal = 0;
    for (const auto& setting : phase_setting) {
        Data input = {setting, max_thruster_signal};
        max_thruster_signal = run_program_from_source(src, input).back();
    }
    return max_thruster_signal;
}

int Circuit::run_program_on_amplifier_id(Program& program, Memory::size_type amplifier_id) {
    std::unique_ptr<Instruction> inst;
    auto& amp = program.at(amplifier_id);
    auto iter = amp.memory.begin();
    while (iter != amp.memory.end()) {
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
                {
                    inst = Input::from_source(iter, modes);
                    amp.wait_for_input();
                    break;
                }
            case Type::OUTPUT:
                {
                    inst = Output::from_source(iter, modes);
                    inst->execute(amp.memory, amp.input, amp.output);
                    program.at(amp.feed_to).add_to_input(
                        amp.output.back()
                    );
                    amp.output.pop_back();
                    continue;
                }
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
                return 0;
            default:
                break;
        }
        inst->execute(amp.memory, amp.input, amp.output);
    }
    return -1;
}

int Circuit::run_program_with_phase_settings(Memory memory, Memory phase_settings) {
    std::thread threads[5];
    auto program = register_amplifiers(phase_settings, memory);
    for (int i = 0; i < 5; i++) {
        threads[i] = std::thread(
            Circuit::run_program_on_amplifier_id,
            std::ref(program),
            phase_settings[i]
        );
    }
    for (int i = 0; i < 5; i++) {
        threads[i].join();
    }
    // After the program terminates, the result is stored in input of the first
    // amplifier.
    return program.at(phase_settings[0]).input.back();
}

Program Circuit::register_amplifiers(const Memory& phase_settings, const Memory& memory) {
    Program program;
    for (Memory::size_type i = 0; i < phase_settings.size(); i++) {
        auto id = phase_settings[i];
        auto idx = (i + 1) % phase_settings.size();
        auto feed_to = phase_settings[idx];
        Data initial_input = {static_cast<int>(id)};
        if (i == 0) {
            // For the first amp, we need to supply first input that is zero.
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

Memory Circuit::get_memory_from_source(const std::string& source) {
    Memory result;
    for (const auto& elem : split_string_by(source, ",")) {
        result.push_back(std::stoi(elem));
    }
    return result;
}

bool Circuit::s_registered = DayFactory::register_day(Circuit::name(), Circuit::create);
