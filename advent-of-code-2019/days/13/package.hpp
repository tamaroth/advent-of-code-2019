#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../day.hpp"
#include "../intcode/intcode.hpp"

class Tile {
public:
	Tile() : x(-1), y(-1), id(-1) {}
	Tile(intcode::Value _x, intcode::Value _y, intcode::Value _id) : x(_x), y(_y), id(_id) {}
	intcode::Value x;
	intcode::Value y;
	intcode::Value id;

	static Tile from_output(intcode::Data& source) {
		auto _x = source.front();
		source.pop_front();
		auto _y = source.front();
		source.pop_front();
		auto _id = source.front();
		source.pop_front();
		return Tile(_x, _y, _id);
	}
};

using Tiles = std::vector<Tile>;

class Package : public Day {
public:
	virtual std::string part_01() override;
	virtual std::string part_02() override;

	static std::unique_ptr<Day> create() {
		return std::make_unique<Package>();
	}
	static std::string name() {
		return "day13";
	}

	Tile get_tile_with_id(const Tiles& tiles, intcode::Value id) {
		auto tile = std::find_if(tiles.begin(), tiles.end(), [&](const auto& tile) {
			return tile.id == id;
		});
		if (tile != tiles.end())
			return *tile;
		return no_tile;
	}

	Tile get_score_tile(const Tiles& tiles) {
		auto score = std::find_if(tiles.begin(), tiles.end(), [](const auto& tile) {
			return tile.x == -1 && tile.y == 0;
		});
		if (score != tiles.end()) {
			return *score;
		}
		return no_tile;
	}

	Tile get_ball_tile(const Tiles& tiles) {
		return get_tile_with_id(tiles, 4);
	}

	Tile get_paddle_tile(const Tiles& tiles) {
		return get_tile_with_id(tiles, 3);
	}

private:
	static bool s_registered;
	static Tile no_tile;
};
