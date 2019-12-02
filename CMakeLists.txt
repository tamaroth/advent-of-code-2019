# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("advent-of-code-2019" C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(Boost_INCLUDE_DIR "D:\\Source\\boost_1.71.0\\")


find_package(Boost 1.71.0)
include_directories(${Boost_INCLUDE_DIRS})
# Include sub-projects.
add_subdirectory ("advent-of-code-2019")
