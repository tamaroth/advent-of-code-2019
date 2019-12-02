#include "day_factory.hpp"

bool DayFactory::register_class(const std::string& name, const CreateMethod& create_method) {
	if (auto it = m_methods.find(name); it == m_methods.end()) {
		m_methods[name] = create_method;
		return true;
	}
	return false;
}

std::unique_ptr<Day> DayFactory::create_class(const std::string& name) {
	if (auto it = m_methods.find(name); it != m_methods.end()) {
		return it->second();
	}
	return nullptr;
}

std::map<std::string, DayFactory::CreateMethod> DayFactory::m_methods;
