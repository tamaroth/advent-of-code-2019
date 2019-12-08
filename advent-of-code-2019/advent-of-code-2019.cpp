// advent-of-code-2019.cpp : Defines the entry point for the application.
//

#include <iostream>

#include "advent-of-code-2019.hpp"
#include "days/day_factory.hpp"

int main()
{
	// I should add here some command line processing, but for now I'm too lazy.
	auto d = DayFactory::create_day("day06");
	if (d != nullptr)
	{
		std::cout << d->part_01() << std::endl;
		std::cout << d->part_02() << std::endl;
	}
	return 0;
}
