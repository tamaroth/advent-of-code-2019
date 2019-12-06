// advent-of-code-2019.cpp : Defines the entry point for the application.
//

#include "advent-of-code-2019.hpp"
#include "days/day_factory.hpp"

int main()
{
	// I should add here some command line processing, but for now I'm too lazy.
	auto d = DayFactory::create_day("day05");
	if (d != nullptr)
	{
		d->part_01();
		d->part_02();
	}
	return 0;
}
