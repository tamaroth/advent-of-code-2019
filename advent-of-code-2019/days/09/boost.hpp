#pragma once

#include "../day.hpp"
#include "../intcode/intcode.hpp"


class Boost : public Day {
	virtual std::string part_01() override;
	virtual std::string part_02() override;

	static std::unique_ptr<Day> create();
	static std::string name();

	intcode::Value run_boost_program_with_input(const intcode::Data& input);
private:
	static bool s_registered;
};
