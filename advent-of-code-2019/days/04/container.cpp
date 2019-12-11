#include "../day_factory.hpp"
#include "container.hpp"


std::string Container::part_01() {
	return std::to_string(compute_number_of_viable_passwords(false));
}

std::string Container::part_02() {
	return std::to_string(compute_number_of_viable_passwords(true));
}

int Container::compute_number_of_viable_passwords(bool must_have_two_adjecent_digits) {
	int passwords = 0;
	for (current = start_value; current < end_value; current++) {
		if (is_current_password_viable(must_have_two_adjecent_digits)) {
			passwords++;
		}
	}
	return passwords;
}

std::unique_ptr<Day> Container::create() {
	return std::make_unique<Container>();
}

std::string Container::name() {
	return "day04";
}

bool Container::is_current_password_viable(bool part2) const {
	bool has_adjecent_values = false;
	bool has_exactly_two_adjecent = false;

	int adjecent_count = 0;
	auto value = current;

	auto reminder = value % 10;
	value /= 10;
	do {
		auto new_reminder = value % 10;
		if (reminder < new_reminder)
			return false;
		if (new_reminder == reminder) {
			has_adjecent_values = true;
			adjecent_count++;
		} else {
			if (!has_exactly_two_adjecent && adjecent_count == 1) {
				has_exactly_two_adjecent = true;
			}
			adjecent_count = 0;
		}
		reminder = new_reminder;
		value /= 10;
	} while (value);

	if (part2) {
		if (!has_exactly_two_adjecent && adjecent_count == 1) {
			return true;
		}
		return has_exactly_two_adjecent;
	}
	return has_adjecent_values;
}

bool Container::s_registered = DayFactory::register_day(Container::name(), Container::create);
