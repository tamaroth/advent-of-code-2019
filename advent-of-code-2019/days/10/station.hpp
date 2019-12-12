# pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "../day.hpp"

// Forward declarations.
class Point;
class PointHasher;

// Type definitions.
using Points = std::vector<Point>;
using PointDistance = std::map<double, Point>;
using StarMap = std::map<double, PointDistance>;

class Point {
public:
	Point();
	Point(int64_t _id, int64_t _x, int64_t _y, double _r, double _phi);

public:
	int64_t id;
	int64_t x;
	int64_t y;
	double r;
	double phi;

	bool operator==(const Point& other) const;

	static Point from_cartesian(int64_t _x, int64_t _y, std::optional<int64_t> _id = std::nullopt);
};

class Station : public Day {
public:
	virtual std::string part_01() override;
	virtual std::string part_02() override;

	static std::unique_ptr<Day> create();
	static std::string name();

private:
	void load_points_from_string(const std::vector<std::string>& src);
	int64_t get_number_of_visible_asteroids_for_point(const Point& point);
	void transorm_map();
	void generate_starmap();
	Point blow_up_n_asteroids(int n);

private:
	static bool s_registered;
	Point station;
	Points points;
	StarMap sm;
	std::vector<std::string> src = {
		"#.#.###.#.#....#..##.#....",
		".....#..#..#..#.#..#.....#",
		".##.##.##.##.##..#...#...#",
		"#.#...#.#####...###.#.#.#.",
		".#####.###.#.#.####.#####.",
		"#.#.#.##.#.##...####.#.##.",
		"##....###..#.#..#..#..###.",
		"..##....#.#...##.#.#...###",
		"#.....#.#######..##.##.#..",
		"#.###.#..###.#.#..##.....#",
		"##.#.#.##.#......#####..##",
		"#..##.#.##..###.##.###..##",
		"#..#.###...#.#...#..#.##.#",
		".#..#.#....###.#.#..##.#.#",
		"#.##.#####..###...#.###.##",
		"#...##..#..##.##.#.##..###",
		"#.#.###.###.....####.##..#",
		"######....#.##....###.#..#",
		"..##.#.####.....###..##.#.",
		"#..#..#...#.####..######..",
		"#####.##...#.#....#....#.#",
		".#####.##.#.#####..##.#...",
		"#..##..##.#.##.##.####..##",
		".##..####..#..####.#######",
		"#.#..#.##.#.######....##..",
		".#.##.##.####......#.##.##",
	};
};
