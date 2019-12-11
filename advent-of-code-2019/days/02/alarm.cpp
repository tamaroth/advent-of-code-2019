#include "../day_factory.hpp"
#include "../intcode/intcode.hpp"
#include "../utils.hpp"
#include "alarm.hpp"


std::string Alarm::part_01() {
	auto memory = intcode::get_memory_from_string(source);
	auto program = intcode::get_program_for_memory_with_patched_data(memory, {12, 2});
	intcode::run_program_on_computer_with_id(program, 0);
	return std::to_string(program.at(0).cpu.memory[0]);
}

std::string Alarm::part_02() {
	for (int first = 0; first < 99; first++) {
		for (int second = 0; second < 99; second++) {
			auto memory = intcode::get_memory_from_string(source);
			auto program = intcode::get_program_for_memory_with_patched_data(memory, {first, second});
			intcode::run_program_on_computer_with_id(program, 0);
			if (program.at(0).cpu.memory[0] == 19690720) {
				return std::to_string(first) + int_to_str(second);
			}
		}
	}
	return std::string();
}

std::unique_ptr<Day> Alarm::create() {
	return std::make_unique<Alarm>();
}

std::string Alarm::name() {
	return "day02";
}

bool Alarm::s_registered = DayFactory::register_day(Alarm::name(), Alarm::create);
