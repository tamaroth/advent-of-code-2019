#include <iostream>
#include <thread>
#include "../10/station.hpp"
#include "../day_factory.hpp"
#include "../intcode/intcode.hpp"
#include "police.hpp"

intcode::Hooks hooks = {
	{
		intcode::Type::INPUT,
		[](intcode::Program&, intcode::Computer& comp, std::unique_ptr<intcode::Instruction>& inst) {
			comp.wait_for_input();
			inst->execute(comp.cpu);
		}
	},
	{
		intcode::Type::OUTPUT,
		[](intcode::Program& program, intcode::Computer& comp, std::unique_ptr<intcode::Instruction>& inst) {
			inst->execute(comp.cpu);
			program.at(comp.feed_to).add_to_input(comp.cpu.output.back());
			comp.cpu.output.pop_back();
		}
	},
	{
		intcode::Type::STOP,
		[](intcode::Program& program, intcode::Computer& comp, std::unique_ptr<intcode::Instruction>&) {
			program.at(comp.feed_to).add_to_input(-1);
		}
	}
};

std::map<Robot, intcode::Value, RobotCompare> surface;

Robot::Robot(int64_t _x, int64_t _y) : Point(100*_x + _y, _x, _y, 0, 0), direction(0, -1) {}

void Robot::adjust_direction(intcode::Value value) {
	std::map<Direction, std::map<intcode::Value, Direction>> decision_tree = {
		{
			{0, -1},
			{
				{0, {-1, 0}},
				{1, {1, 0}},
			}
		},
		{
			{0, 1},
			{
				{0, {1, 0}},
				{1, {-1, 0}},
			}
		},
		{
			{-1, 0},
			{
				{0, {0, 1}},
				{1, {0, -1}},
			}
		},
		{
			{1, 0},
			{
				{0, {0, -1}},
				{1, {0, 1}},
			}
		}
	};
	direction = decision_tree[direction][value];
	x += direction.first;
	y += direction.second;
	id = 100*x + y;
}

void run_robot(intcode::Program& program) {
	Robot robot(0, 0);
	auto& brain = program.at(1);
	brain.wait_for_input_and_pop();
	for (;;) {
		auto color = brain.wait_for_input_and_pop();
		if (color == -1) {
			break;
		}
		surface[robot] = color;
		auto direction = brain.wait_for_input_and_pop();
		robot.adjust_direction(direction);
		if (surface.contains(robot)) {
			color = surface[robot];
		} else {
			color = 0;
		}
		program.at(0).add_to_input(color);
	}
}

std::string Police::part_01() {
	auto memory = intcode::get_memory_from_string(src);
	auto program = intcode::get_program_for_memory_with_phase_settings(memory, {0, 1});
	program.at(0).cpu.input.pop_back();
	auto brain = std::thread(
		intcode::run_program_on_computer_with_id,
		std::ref(program),
		0,
		hooks
	);
	auto robot = std::thread(
		run_robot,
		std::ref(program)
	);
	brain.join();
	robot.join();

	return std::to_string(surface.size());
}

std::string Police::part_02() {
	surface.clear();
	auto memory = intcode::get_memory_from_string(src);
	auto program = intcode::get_program_for_memory_with_phase_settings(memory, {0, 1});
	program.at(0).cpu.input.pop_back();
	program.at(0).cpu.input[0] = 1;
	auto brain = std::thread(
		intcode::run_program_on_computer_with_id,
		std::ref(program),
		0,
		hooks
	);
	auto robot = std::thread(
		run_robot,
		std::ref(program)
	);
	brain.join();
	robot.join();

	int64_t smallest_x = std::numeric_limits<int64_t>::max();
	int64_t smallest_y = std::numeric_limits<int64_t>::max();
	int64_t max_x = 0;
	int64_t max_y = 0;
	for (const auto& px : surface) {
		smallest_x = std::min(smallest_x, px.first.x);
		smallest_y = std::min(smallest_y, px.first.y);
		max_x = std::max(max_x, px.first.x);
		max_y = std::max(max_y, px.first.y);
	}

	std::string result = "";
	for (int64_t y = smallest_y; y <= max_y; y++) {
		for (int64_t x = smallest_x; x <= max_x; x++) {
			Robot r(x, y);
			if (surface.contains(r)) {
				if (surface[r] == 1) {
					result += 'O';
				} else {
					result += ' ';
				}
			} else {
				result += ' ';
			}
		}
		result += '\n';
	}

	return result;
}

std::unique_ptr<Day> Police::create() {
	return std::make_unique<Police>();
}

std::string Police::name() {
	return "day11";
}

bool Police::s_registered = DayFactory::register_day(Police::name(), Police::create);
