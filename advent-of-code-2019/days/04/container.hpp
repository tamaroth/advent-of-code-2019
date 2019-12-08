#pragma once

#include <memory>
#include <string>

#include "../day.hpp"


class Container : public Day {
public:
	virtual std::string part_01() override;
	virtual std::string part_02() override;

	static std::unique_ptr<Day> create();
	static std::string name();

private:
	int compute_number_of_viable_passwords(bool must_have_two_adjecent_digits);
	bool is_current_password_viable(bool part2) const;

private:
	uint64_t start_value = 278384;
	uint64_t end_value = 824795;
	uint64_t current;
	static bool s_registered;
};
