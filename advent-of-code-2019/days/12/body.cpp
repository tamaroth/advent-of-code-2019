#include <iostream>
#include <numeric>

#include "../day_factory.hpp"
#include "body.hpp"

std::string Body::part_01() {
	std::vector<std::string> sources = {
		"<x=-9, y=-1, z=-1>",
		"<x=2, y=9, z=5>",
		"<x=10, y=18, z=-12>",
		"<x=-6, y=15, z=-7>",
	};
	auto bodies = get_bodies_from_strings(sources);
	int steps = 1000;
	while (steps-- > 0) {
		compute_step(bodies);
	}
	int64_t system_energy = 0;
	for (const auto& body : bodies) {
		system_energy += (body.gravity.energy() * body.velocity.energy());
	}
	return std::to_string(system_energy);
}

std::string Body::part_02() {
	std::vector<std::string> source = {
		"<x=-9, y=-1, z=-1>",
		"<x=2, y=9, z=5>",
		"<x=10, y=18, z=-12>",
		"<x=-6, y=15, z=-7>",
	};
	auto starting = get_bodies_from_strings(source);
	auto current = starting;
	int64_t cycle = 0;
	std::optional<int64_t> x_cycle = std::nullopt;
	std::optional<int64_t> y_cycle = std::nullopt;
	std::optional<int64_t> z_cycle = std::nullopt;
	while (!x_cycle || !y_cycle || !z_cycle) {
		compute_step(current);
		cycle++;
		if (!x_cycle && is_x_coordinate_cycled(starting, current)) {
			x_cycle = cycle;
		}
		if (!y_cycle && is_y_coordinate_cycled(starting, current)) {
			y_cycle = cycle;
		}
		if (!z_cycle && is_z_coordinate_cycled(starting, current)) {
			z_cycle = cycle;
		}
	}
	return std::to_string(std::lcm(*x_cycle, std::lcm(*y_cycle, *z_cycle)));
}

std::unique_ptr<Day> Body::create() {
	return std::make_unique<Body>();
}

std::string Body::name() {
	return "day12";
}

bool Body::s_registered = DayFactory::register_day(Body::name(), Body::create);
