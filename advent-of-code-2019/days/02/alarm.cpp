#include <iostream>

#include "alarm.hpp"
#include "../day_factory.hpp"

std::string Alarm::part_01() {
	auto program = get_program_from_string(source);
	patch_program(program, {12, 2});
	run_program(program);
	return std::to_string(program[0]);
}

std::string Alarm::part_02() {
	for (int first = 0; first < 99; first++) {
		for (int second = 0; second < 99; second++) {
			auto program = get_program_from_string(source);
			patch_program(program, { first, second });
			run_program(program);
			if (program[0] == 19690720) {
				return std::to_string(first) + ", " + std::to_string(second);
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

std::vector<int> Alarm::get_program_from_string(const std::string& source) {
	std::vector<int> parsed;
	auto partially_parsed = split_string_by(source, ",");
	for (const auto& elem : partially_parsed) {
		parsed.push_back(std::stoi(elem));
	}
	return parsed;
}

void Alarm::patch_program(std::vector<int>& source, std::vector<int> patch) {
	source[1] = patch[0];
	source[2] = patch[1];
}

void Alarm::run_program(std::vector<int>& source) {
	auto cur= source.begin();
	for (; cur != source.end() && *cur != 99;) {
		auto inst = Instruction::read_instruction(cur);
		switch (inst.opcode)
		{
		case 1:
			source[inst.parameters.output] = source[inst.parameters.first] + source[inst.parameters.second];
			break;
		case 2:
			source[inst.parameters.output] = source[inst.parameters.first] * source[inst.parameters.second];
			break;
		default:
			break;
		}
	}
}

bool Alarm::s_registered = DayFactory::register_day(Alarm::name(), Alarm::create);
