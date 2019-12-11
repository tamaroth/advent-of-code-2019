#pragma once

#include <string>
#include <vector>

class Day {
public:
	Day() = default;
	~Day() = default;

	virtual std::string part_01() = 0;
	virtual std::string part_02() = 0;
};
