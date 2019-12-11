#include <algorithm>

#include "../day_factory.hpp"
#include "circuit.hpp"


std::string Circuit::part_01() {
	intcode::Memory phase_setting = {0, 1, 2, 3, 4};
	int64_t max_thruster_signal = 0;
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
	auto memory = intcode::get_memory_from_string(src);
	intcode::Memory phase_settings = {5, 6, 7, 8, 9};
	int64_t max_thruster_signal = 0;
	do {
		max_thruster_signal = std::max(
			max_thruster_signal,
			run_program_with_phase_settings(memory, phase_settings)
		);
	} while (std::next_permutation(phase_settings.begin(), phase_settings.end()));
	return std::to_string(max_thruster_signal);
}

int64_t Circuit::get_thruster_signal_for_phase_setting_sequence(const intcode::Memory& phase_setting) {
	int64_t max_thruster_signal = 0;
	for (const auto& setting : phase_setting) {
		intcode::Data input = {setting, max_thruster_signal};
		auto memory = intcode::get_memory_from_string(src);
		auto program = intcode::get_program_for_memory_with_input_data(memory, input);
		intcode::run_program_on_computer_with_id(program, 0);
		max_thruster_signal = program.at(0).cpu.output.back();
	}
	return max_thruster_signal;
}

int64_t Circuit::run_program_with_phase_settings(intcode::Memory memory, intcode::Memory phase_settings) {
	intcode::Hooks hooks = {
		{
			intcode::Type::INPUT,
			[&](intcode::Program& /*program*/, intcode::Computer& comp, std::unique_ptr<intcode::Instruction>& inst) {
				comp.wait_for_input();
				inst->execute(comp.cpu);
			}
		},
		{
			intcode::Type::OUTPUT,
			[&](intcode::Program& program, intcode::Computer& comp, std::unique_ptr<intcode::Instruction>& inst) {
				inst->execute(comp.cpu);
				program.at(comp.feed_to).add_to_input(comp.cpu.output.back());
				comp.cpu.output.pop_back();
			}
		}
	};
	auto program = intcode::get_program_for_memory_with_phase_settings(memory, phase_settings);
	intcode::run_program_with_phase_settings(program, phase_settings, hooks);
	return program.at(phase_settings[0]).cpu.input.back();
}

std::unique_ptr<Day> Circuit::create() {
	return std::make_unique<Circuit>();
}

std::string Circuit::name() {
	return "day07";
}

bool Circuit::s_registered = DayFactory::register_day(Circuit::name(), Circuit::create);
