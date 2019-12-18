#include <cmath>
#include <iostream>
#include <map>

#include "../day_factory.hpp"
#include "space.hpp"

void Space::generate_n_chemicals(Storage& storage, const Chemicals& chems, const Chemical& chem, int64_t amount) {
	double generated_count = std::ceil(static_cast<double>(amount) / static_cast<double>(chem.amount));
	for (auto [name, per_generate] : chem.requirements) {
		int64_t required_amount = std::llround(std::ceil(double(per_generate) * generated_count));
		if (name == "ORE") {
			storage["ORE"] += required_amount;
			continue;
		}
		if (storage.contains(name)) {
			auto& left = storage.at(name);
			auto to_remove = std::min(required_amount, left);
			left -= to_remove;
			required_amount -= to_remove;
			if (required_amount == 0) {
				continue;
			}
		}
		auto c = chems.at(name);
		generate_n_chemicals(storage, chems, c, required_amount);
		storage[name] -= required_amount;
	}
	storage[chem.name] += std::llround(generated_count) * chem.amount;
}

std::string Space::part_01() {
	set_up();
	generate_n_chemicals(storage, chemicals, chemicals.at("FUEL"), 1);
	return std::to_string(ore = storage["ORE"]);
}

std::string Space::part_02() {
	int64_t lower_bound = 0;
	int64_t upper_bound = std::numeric_limits<int>::max() - 1;
	int64_t result = 0;
	const int64_t t = 1000000000000;
	while (lower_bound <= upper_bound) {
		auto middle = std::llround(std::floor((lower_bound + upper_bound) / 2));
		auto current_ore = get_ore_for_fuel(middle);

		if (current_ore < t) {
			auto next_ore = get_ore_for_fuel(middle + 1);
			if (next_ore >= t) {
				result = middle;
				break;
			}
			lower_bound = middle + 1;
		} else if (current_ore > t) {
			auto prev_ore = get_ore_for_fuel(middle - 1);
			if (prev_ore <= t) {
				result = middle - 1;
				break;
			}
			upper_bound = middle - 1;
		} else {
			result = middle;
			break;
		}
	}
	return std::to_string(result);
}

bool Space::s_registered = DayFactory::register_day(Space::name(), Space::create);
