// advent-of-code-2019.cpp : Defines the entry point for the application.
//

#include <iostream>

#include "advent-of-code-2019.hpp"
#include "days/day_factory.hpp"
#include "days/utils.hpp"


int main()
{
	// I should add here some command line processing, but for now I'm too lazy.
	for (int i = 1; i <= 10; i++) {
		auto day_name = "day" + int_to_str(i);
		auto day = DayFactory::create_day(day_name);
		std::cout << "Solution for " << day_name << std::endl;
		if (day != nullptr) {
			std::cout << day->part_01() << std::endl;
			std::cout << day->part_02() << std::endl;
		}
	}
	return 0;
}
