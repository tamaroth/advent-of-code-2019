#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../day.hpp"

class SpaceObject {
public:
	std::string name;
	SpaceObject* orbits_around = nullptr;
	std::vector<SpaceObject*> objects_orbitting;

	static SpaceObject from_str(const std::string& str) {
		SpaceObject orb;
		orb.name = str;
		orb.orbits_around = nullptr;
		return orb;
	}
};


class Orbits : public Day {
public:
	virtual void part_01() override;
	virtual void part_02() override;

	static std::unique_ptr<Day> create();
	static std::string name();

private:
	void load_map(const std::string& src);
	std::vector<std::string> split_string_by(const std::string& src, const std::string& delimiter);

private:
	static bool s_registered;
	std::map<std::string, SpaceObject> starmap;
};
