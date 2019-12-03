#pragma once

#include <map>
#include <memory>
#include <string>

#include "day.hpp"

class DayFactory {
	using CreateMethod = std::unique_ptr<Day>(*)();
public:
	DayFactory() = delete;

	static bool register_day(const std::string& name, const CreateMethod& create_method);
	static std::unique_ptr<Day> create_day(const std::string& name);

private:
	static std::map<std::string, CreateMethod> m_methods;
};
