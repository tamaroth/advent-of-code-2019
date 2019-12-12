#include <cmath>
#include <iostream>
#include <set>

#include "../day_factory.hpp"
#include "station.hpp"


Point::Point() : id(0), x(0), y(0), r(0.0), phi(0.0) {}

Point::Point(int64_t _id, int64_t _x, int64_t _y, double _r, double _phi) :
	id(_id), x(_x), y(_y), r(_r), phi(_phi) {}

Point Point::from_cartesian(int64_t _x, int64_t _y, std::optional<int64_t> _id) {
	if (!_id) {
		_id = 100 * _x + _y;
	}
	auto _r = std::sqrt(std::pow(_x, 2) + std::pow(_y, 2));
	auto _phi = std::atan2(_y, _x);
	return Point(*_id, _x, _y, _r, _phi);
}

bool Point::operator==(const Point& other) const {
	return id == other.id;
}


std::string Station::part_01() {
	int64_t max = 0;
	load_points_from_string(src);
	for (const auto& point : points) {
		auto visible_asteroids = get_number_of_visible_asteroids_for_point(point);
		if (visible_asteroids > max) {
			max = visible_asteroids;
			station = point;
		}
	}
	return std::to_string(max);
}

std::string Station::part_02() {
	load_points_from_string(src);
	transorm_map();
	generate_starmap();
	auto last = blow_up_n_asteroids(200);	
	return std::to_string(last.id);
}

void Station::load_points_from_string(const std::vector<std::string>& src) {
	uint64_t row = 0;
	points.clear();
	for (const auto& line : src) {
		uint64_t column = 0;
		for (const auto& ch : line) {
			if (ch == '#') {
				points.push_back(Point::from_cartesian(column, row));
			}
			column++;
		}
		row++;
	}
}

int64_t Station::get_number_of_visible_asteroids_for_point(const Point& point) {
	std::set<double> visible_asteroids;
	for (const auto& p : points) {
		if (p.id == point.id) {
			continue;
		}
		auto px = p.x - point.x;
		auto py = p.y - point.y;
		auto pc = Point::from_cartesian(px, py);
		visible_asteroids.insert(pc.phi);
	}
	return static_cast<int64_t>(visible_asteroids.size());
}

void Station::transorm_map() {
	Points relative_points;
	for (const auto& point : points) {
		if (point == station) {
			continue;
		}
		auto px = point.x - station.x;
		auto py = point.y - station.y;
		auto id = 100 * point.x + point.y;
		auto pt = Point::from_cartesian(px, py, id);
		relative_points.push_back(std::move(pt));
	}
	points = relative_points;
}

void Station::generate_starmap() {
	for (const auto& pt : points) {
		if (!sm.contains(pt.phi)) {
			sm[pt.phi] = PointDistance();
		}
		sm.at(pt.phi)[pt.r] = pt;
	}
}

Point Station::blow_up_n_asteroids(int n) {
	Point last;
	auto neg_pi_half = std::atan2(-1, 0);
	if (auto iter = sm.find(neg_pi_half); iter != sm.end()) {
		int cnt = 0;
		while (cnt != n) {
			if (iter == sm.end()) {
				iter = sm.begin();
			}
			auto& distances = iter->second;
			if (distances.size()) {
				auto it = distances.begin();
				cnt++;
				last = it->second;
				distances.erase(it);
			}
			iter++;
		}
	}
	return last;
}

std::unique_ptr<Day> Station::create() {
	return std::make_unique<Station>();
}

std::string Station::name() {
	return "day10";
}

bool Station::s_registered = DayFactory::register_day(Station::name(), Station::create);
