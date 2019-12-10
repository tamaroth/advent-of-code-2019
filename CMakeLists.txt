# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("advent-of-code-2019" C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (UNIX)
	string(APPEND CMAKE_CXX_FLAGS " -pedantic")
	string(APPEND CMAKE_CXX_FLAGS " -Wfatal-errors")
	string(APPEND CMAKE_CXX_FLAGS " -Wall -Wextra")
	string(APPEND CMAKE_CXX_FLAGS " -Wcast-align")
	string(APPEND CMAKE_CXX_FLAGS " -Wcast-qual")
	string(APPEND CMAKE_CXX_FLAGS " -Wconversion")
	string(APPEND CMAKE_CXX_FLAGS " -Wfloat-equal")
	string(APPEND CMAKE_CXX_FLAGS " -Wold-style-cast")
	string(APPEND CMAKE_CXX_FLAGS " -Wswitch-default")
	string(APPEND CMAKE_CXX_FLAGS " -Wuninitialized")
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		string(APPEND CMAKE_CXX_FLAGS " -Wzero-as-null-pointer-constant")
		string(APPEND CMAKE_CXX_FLAGS " -Wsuggest-override")
		string(APPEND CMAKE_CXX_FLAGS " -Wno-maybe-uninitialized")
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		string(APPEND CMAKE_CXX_FLAGS " -Wconditional-uninitialized")
		string(APPEND CMAKE_CXX_FLAGS " -Wheader-hygiene")
		string(APPEND CMAKE_CXX_FLAGS " -Wnon-literal-null-conversion")
		string(APPEND CMAKE_CXX_FLAGS " -Wreserved-id-macro")
		string(APPEND CMAKE_CXX_FLAGS " -Wsometimes-uninitialized")
		string(APPEND CMAKE_CXX_FLAGS " -Wunreachable-code")
		string(APPEND CMAKE_CXX_FLAGS " -Wunused-exception-parameter")
	endif()
endif()

find_package(Boost)
if(Boost_FOUND)
	include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
endif()

# Include sub-projects.
add_subdirectory ("advent-of-code-2019")
