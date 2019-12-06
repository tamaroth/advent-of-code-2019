#include <algorithm>
#include <ostream>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "wires.hpp"
#include "../day_factory.hpp"

void Wires::part_01() {
	travel_steps(steps_01 = get_path_from_string(path_01));
	travel_steps(steps_02 = get_path_from_string(path_02));
	intersections = panel.intersections();
	std::cout << get_closest_intersection_to_start().get_distance_from_start() << std::endl;
}

void Wires::part_02() {
	travel_steps_and_count_distance_to_intersections(steps_01);
	travel_steps_and_count_distance_to_intersections(steps_02);
	std::cout << get_shortest_combined_distance_to_intersection();
}

std::unique_ptr<Day> Wires::create()
{
	return std::make_unique<Wires>();
}

std::string Wires::name()
{
	return "day03";
}

std::vector<Wires::Step> Wires::get_path_from_string(const std::string& string_path) {
	std::vector<std::string> partially_parsed;
	std::vector<Wires::Step> result;
	auto identity = string_path[0];
	auto path = string_path.substr(1);
	boost::split(partially_parsed, path, boost::is_any_of(","));
	for (const auto& str_step : partially_parsed) {
		result.push_back(Wires::Step::from_string(str_step, identity));
	}
	return result;
}

void Wires::travel_steps(const Steps& steps) {
	panel.reset_pos();
	for (const auto& step : steps) {
		travel_step(step);
	}
}

void Wires::travel_steps_and_count_distance_to_intersections(const Steps& steps) {
	panel.reset_pos();
	int distance = 0;
	for (const auto& step : steps) {
		for (int i = 0; i < step.distance; i++) {
			auto next_pos = panel.get_next_pos(step.direction);
			distance++;
			if (panel.is_intersection(next_pos)) {
				intersections[next_pos].insert(std::pair<const char, int>(step.identity, distance));
			}
		}
	}
}

void Wires::travel_step(const Step& step) {
	for (int i = 0; i < step.distance; i++) {
		panel.mark_step(step);
	}
}

int Wires::get_combined_distance(const Intersection& intersection) const {
	int result = 0;
	for (const auto& p : intersection) {
		result += p.second;
	}
	return result;
}

int Wires::get_shortest_combined_distance_to_intersection() const {
	auto pred = [&](const auto& lhs, const auto& rhs) {
		if (lhs.second.size() != 2 || rhs.second.size() != 2) {
			return false;
		}
		return get_combined_distance(lhs.second) < get_combined_distance(rhs.second);
	};

	return get_combined_distance(
		std::min_element(
			intersections.begin(),
			intersections.end(),
			pred
		)->second
	);

}

Wires::Coordinate Wires::get_closest_intersection_to_start() {
	return std::min_element(
		intersections.begin(),
		intersections.end(),
		[](const auto& first, const auto& second) {
			return first.first.get_distance_from_start() < second.first.get_distance_from_start();
		}
	)->first;
}

Wires::Step Wires::Step::from_string(const std::string& str, char identity) {
	Step step;
	step.direction = str[0];
	step.identity = identity;
	step.distance = std::stoi(str.substr(1));
	return step;
}

int Wires::Coordinate::get_distance_from_start() const {
	return static_cast<int>(std::abs(x) + std::abs(y));
}

Wires::Coordinate Wires::Panel::get_next_pos(char direction) {
	switch (direction)
	{
	case 'R':
		last_pos.x++;
		break;
	case 'U':
		last_pos.y++;
		break;
	case 'L':
		last_pos.x--;
		break;
	case 'D':
		last_pos.y--;
		break;
	default:
		break;
	}
	return last_pos;
}

void Wires::Panel::reset_pos() {
	last_pos = { 0,0 };
}

void Wires::Panel::mark_step(const Step& step) {
	auto next_pos = get_next_pos(step.direction);
	if (panel.contains(next_pos)) {
		panel[next_pos].insert(step.identity);
	} else {
		panel[next_pos] = std::set({ step.identity });
	}
}

Wires::Intersections Wires::Panel::intersections() const {
	Intersections result;
	for (const auto& point : panel) {
		if (point.second.size() > 1) {
			result[point.first] = std::set<std::pair<const char, int>>();
		}
	}
	return result;
}

bool Wires::Panel::is_intersection(const Coordinate& coord) const {
	return panel.at(coord).size() > 1;
}

std::ostream& operator<<(std::ostream& os, const Wires::Coordinate& coords) {
	os << coords.x << " " << coords.y << std::endl;
	return os;
}

bool Wires::s_registered = DayFactory::register_day(Wires::name(), Wires::create);
