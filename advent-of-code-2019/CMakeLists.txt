﻿# CMakeList.txt : CMake project for advent-of-code-2019, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)


# Add source to this project's executable.
add_executable (advent-of-code-2019
	"days/day_factory.cpp"
	"days/utils.cpp"
	"days/01/tyranny.cpp"
	"days/02/alarm.cpp"
	"days/03/wires.cpp"
	"days/04/container.cpp"
	"days/05/asteroids.cpp"
	"days/06/orbits.cpp"
	"days/07/circuit.cpp"
	"days/08/sif.cpp"
	"days/09/boost.cpp"
	"days/10/station.cpp"
	"days/11/police.cpp"
	"days/12/body.cpp"
	"days/13/package.cpp"
	"days/14/space.cpp"
	"days/intcode/intcode.cpp"
	"advent-of-code-2019.cpp"
	"advent-of-code-2019.hpp"
	)

# TODO: Add tests and install targets if needed.
target_link_libraries(advent-of-code-2019 ${Boost_LIBRARIES})
