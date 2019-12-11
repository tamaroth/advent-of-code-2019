#include <ios>
#include <iomanip>
#include <sstream>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "utils.hpp"

std::string int_to_str(int value) {
	std::ostringstream oss;
	oss << std::internal << std::setfill('0') << std::setw(2) << value;
	return oss.str();
}

std::vector<std::string> split_string_by(const std::string& src, const std::string& delimiter) {
	std::vector<std::string> result;
	boost::split(result, src, boost::is_any_of(delimiter));
	return result;
}
