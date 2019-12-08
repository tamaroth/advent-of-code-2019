#pragma once

#include <string>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

class Day {
public:
	Day() = default;
	~Day() = default;

	virtual std::string part_01() = 0;
	virtual std::string part_02() = 0;

	std::vector<std::string> split_string_by(const std::string& src, const std::string& delimiter) {
		std::vector<std::string> result;
		boost::split(result, src, boost::is_any_of(delimiter));
		return result;
	}
};
