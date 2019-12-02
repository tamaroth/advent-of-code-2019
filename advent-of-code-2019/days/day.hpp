#pragma once

#include <string>
#include <map>
#include <iostream>
#include <memory>

class Day {
public:
	Day() = default;
	~Day() = default;

	virtual void part_01() = 0;
	virtual void part_02() = 0;
};
