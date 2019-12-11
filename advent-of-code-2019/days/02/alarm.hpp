#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../day.hpp"

class Alarm : public Day {
public:
	virtual std::string part_01() override;
	virtual std::string part_02() override;

	static std::unique_ptr<Day> create();
	static std::string name();

private:
	static bool s_registered;
	#include "puzzle_input"
};
