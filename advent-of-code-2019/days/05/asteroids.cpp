
#include "../day_factory.hpp"
#include "asteroids.hpp"

std::string Asteroids::part_01() {
	return std::to_string(get_output_of_code_run_with_data({1}));
}

std::string Asteroids::part_02() {
	return std::to_string(get_output_of_code_run_with_data({5}));
}

intcode::Value Asteroids::get_output_of_code_run_with_data(const intcode::Data& input_data) {
	auto memory = intcode::get_memory_from_string(src);
	auto program = intcode::get_program_for_memory_with_input_data(memory, input_data);
	intcode::run_program_on_computer_with_id(program, 0);
	return program.at(0).cpu.output.back();
}

std::unique_ptr<Day> Asteroids::create() {
	return std::make_unique<Asteroids>();
}

std::string Asteroids::name() {
	return "day05";
}

bool Asteroids::s_registered = DayFactory::register_day(Asteroids::name(), Asteroids::create);
