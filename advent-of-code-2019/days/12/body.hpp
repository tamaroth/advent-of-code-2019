#pragma once

#include <map>
#include <memory>
#include <optional>
#include <cstdlib>
#include <string>
#include <regex>

#include "../day.hpp"


class Position {
public:
	Position(int64_t _x, int64_t _y, int64_t _z) : x(_x), y(_y), z(_z) {}
	bool operator==(const Position& other) {
		return x == other.x && y == other.y && z == other.z;
	}
	bool operator!=(const Position& other) {
		return x != other.x || y != other.y || z != other.z;
	}
	int64_t energy() const {
		return std::abs(x) + std::abs(y) + std::abs(z);
	}
	int64_t x;
	int64_t y;
	int64_t z;
};

class Gravity : public Position {
public:
	Gravity(int64_t x, int64_t y, int64_t z) : Position(x, y, z) {}
	static std::optional<Gravity> from_string(const std::string& src) {
		std::regex rx{ R"(<x=(-?\d+)+, y=(-?\d+)+, z=(-?\d+)+>)"};
		std::smatch match;

		if (std::regex_match(src, match, rx)) {
			auto x = std::stoi(match[1]);
			auto y = std::stoi(match[2]);
			auto z = std::stoi(match[3]);
			return {Gravity(x, y, z)};
		}
		return std::nullopt;
	}
};

class Velocity : public Position {
public:
	Velocity(int64_t x, int64_t y, int64_t z) : Position(x, y, z) {}
};

class StellarBody;
using StellarBodies = std::vector<StellarBody>;

class StellarBody {
public:
	StellarBody(const Gravity& _gravity, const Velocity& _velocity) :
		gravity(_gravity), velocity(_velocity) {}

	void apply_gravity(const StellarBodies& bodies) {
		for (const auto& body : bodies) {
			if (gravity == body.gravity) {
				continue;
			}
			if (gravity.x != body.gravity.x) {
				velocity.x += (gravity.x < body.gravity.x ? 1 : -1);
			}
			if (gravity.y != body.gravity.y) {
				velocity.y += (gravity.y < body.gravity.y ? 1 : -1);
			}
			if (gravity.z != body.gravity.z) {
				velocity.z += (gravity.z < body.gravity.z ? 1 : -1);
			}
		}
	}
	void apply_velocity() {
		gravity.x += velocity.x;
		gravity.y += velocity.y;
		gravity.z += velocity.z;
	}

	static std::optional<StellarBody> from_string(const std::string& str) {
		auto gravity = Gravity::from_string(str);
		if (!gravity) {
			return std::nullopt;
		}
		auto velocity = Velocity(0, 0, 0);
		return {StellarBody(*gravity, velocity)};
	}

	Gravity gravity;
	Velocity velocity;
};

class Body : public Day {
public:
	virtual std::string part_01() override;
	virtual std::string part_02() override;

	static std::unique_ptr<Day> create();
	static std::string name();

private:
	StellarBodies get_bodies_from_strings(const std::vector<std::string>& strings) {
		StellarBodies bodies;
		for (const auto& src : strings) {
			auto body = StellarBody::from_string(src);
			if (body) {
				bodies.push_back(*body);
			}
		}
		return bodies;
	}
	void compute_step(StellarBodies& bodies) {
		for (auto& body : bodies) {
			body.apply_gravity(bodies);
		}
		for (auto& body : bodies) {
			body.apply_velocity();
		}
	}
	bool is_x_coordinate_cycled(const StellarBodies& starting, const StellarBodies& current) const {
		return is_coordinate_cycled(
			starting,
			current,
			[](const auto& body) { return body.gravity.x; },
			[](const auto& body) { return body.velocity.x; }
		);
	}
	bool is_y_coordinate_cycled(const StellarBodies& starting, const StellarBodies& current) const {
		return is_coordinate_cycled(
			starting,
			current,
			[](const auto& body) { return body.gravity.y; },
			[](const auto& body) { return body.velocity.y; }
		);
	}
	bool is_z_coordinate_cycled(const StellarBodies& starting, const StellarBodies& current) const {
		return is_coordinate_cycled(
			starting,
			current,
			[](const auto& body) { return body.gravity.z; },
			[](const auto& body) { return body.velocity.z; }
		);
	}
	template<typename Gravity, typename Velocity>
	bool is_coordinate_cycled(const StellarBodies& starting, const StellarBodies& current,
			Gravity&& get_gravity, Velocity&& get_velocity) const {
		bool result = true;
		for (int i = 0; i < 4; i++) {
			if (get_gravity(current[i]) != get_gravity(starting[i]) || get_velocity(current[i]) != 0) {
				result = false;
				break;
			}
		}
		return result;
	}
private:
	static bool s_registered;
};
