#include <iostream>

#include "../day_factory.hpp"
#include "../utils.hpp"
#include "orbits.hpp"


std::string Orbits::part_01() {
	#include "puzzle_input"
	load_map(src);
	int total_orbits = 0;
	for (const auto& [name, orbit] : starmap) {
		auto current = orbit.orbits_around;

		while (current != nullptr) {
			total_orbits++;
			current = current->orbits_around;
		}
	}
	return std::to_string(total_orbits);
}

std::string Orbits::part_02() {
	std::vector<std::string> visited;
	auto current = starmap.at("YOU").orbits_around;
	while (current != nullptr) {
		visited.push_back(current->name);
		current = current->orbits_around;
	}

	current = starmap.at("SAN").orbits_around;
	int traversed = 0;
	while (current != nullptr) {
		auto element = std::find(visited.begin(), visited.end(), current->name);
		if (element != visited.end()) {
			return std::to_string(traversed + std::distance(visited.begin(), element));
		}
		traversed++;
		current = current->orbits_around;
	}
	return std::string("error");
}

std::unique_ptr<Day> Orbits::create() {
	return std::make_unique<Orbits>();
}

std::string Orbits::name() {
	return "day06";
}

void Orbits::load_map(const std::string& src) {
	for (const auto& line : split_string_by(src, "\n")) {
		auto orbit_names = split_string_by(line, ")");

		for (const auto& orbit : orbit_names) {
			starmap.try_emplace(orbit, SpaceObject::from_str(orbit));
		}

		starmap[orbit_names[1]].orbits_around = &starmap[orbit_names[0]];
		starmap[orbit_names[0]].objects_orbitting.push_back(&starmap[orbit_names[1]]);
	}
}

bool Orbits::s_registered = DayFactory::register_day(
	Orbits::name(),
	Orbits::create
);
