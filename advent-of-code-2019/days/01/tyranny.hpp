#pragma once

#include <memory>
#include <string>

#include "../day.hpp"


class Tyranny : public Day {
public:
	virtual void part_01() override;
	virtual void part_02() override;

	static std::unique_ptr<Day> create();
	static std::string name();

private:
	int64_t get_fuel_requirements_for_mass(const int64_t& mass);
	int64_t get_fuel_for_mass(const int64_t& mass);

private:
	static bool s_registered;
private:
	std::vector<int> source{
		#include "part_01_input.dat"
	};
};
