#include <iostream>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "alarm.hpp"
#include "../day_factory.hpp"

void Alarm::part_01() {
	auto program = get_program_from_string(source);
	patch_program(program, {12, 2});
	run_program(program);
	std::cout << program[0] << std::endl;
}

void Alarm::part_02() {
	for (int first = 0; first < 99; first++) {
		for (int second = 0; second < 99; second++) {
			auto program = get_program_from_string(source);
			patch_program(program, { first, second });
			run_program(program);
			if (program[0] == 19690720) {
				std::cout << first << ", " << second << std::endl;
				return;
			}
		}
	}
}

std::unique_ptr<Day> Alarm::create() {
	return std::make_unique<Alarm>();
}

std::string Alarm::name() {
	return "day02";
}

std::vector<int> Alarm::get_program_from_string(std::string_view source) {
	std::vector<int> parsed;
	std::vector<std::string> partially_parsed;
	boost::split(partially_parsed, source, boost::is_any_of(","));
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
