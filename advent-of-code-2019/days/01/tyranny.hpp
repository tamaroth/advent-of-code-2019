#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../day.hpp"


class Tyranny : public Day {
public:
	virtual std::string part_01() override;
	virtual std::string part_02() override;

	static std::unique_ptr<Day> create();
	static std::string name();

private:
	int64_t get_fuel_requirements_for_mass(const int64_t& mass);
	int64_t get_fuel_for_mass(const int64_t& mass);

private:
	static bool s_registered;
private:
	std::vector<int> source{
		#include "puzzle_input"
	};
};
