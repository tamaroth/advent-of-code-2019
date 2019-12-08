#include <cmath>
#include <iostream>

#include "tyranny.hpp"
#include "../day_factory.hpp"


std::string Tyranny::part_01() {
	int64_t total_fuel = 0;
	for (const auto& mass : source) {
		total_fuel += get_fuel_requirements_for_mass(mass);
	}
	return std::to_string(total_fuel);
}

std::string Tyranny::part_02() {
	int64_t total_fuel = 0;
	for (const auto& mass : source) {
		total_fuel += get_fuel_for_mass(mass);
	}
	return std::to_string(total_fuel);
}

std::unique_ptr<Day> Tyranny::create() {
	return std::make_unique<Tyranny>();
}

std::string Tyranny::name() {
	return "day01";
}

int64_t Tyranny::get_fuel_requirements_for_mass(const int64_t& mass) {
	return static_cast<uint64_t>(std::floor(mass / 3)) - 2;
}

int64_t Tyranny::get_fuel_for_mass(const int64_t& mass) {
	int64_t fuel = get_fuel_requirements_for_mass(mass);
	if (fuel > 0) {
		fuel += get_fuel_for_mass(fuel);
		return fuel;
	}
	return 0;
}

bool Tyranny::s_registered = DayFactory::register_day(Tyranny::name(), Tyranny::create);
