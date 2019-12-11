#include <iostream>
#include <limits>

#include "sif.hpp"
#include "../day_factory.hpp"

bool SIF::s_registered = DayFactory::register_day(SIF::name(), SIF::create);

std::unique_ptr<Day> SIF::create() {
	return std::make_unique<SIF>();
}

std::string SIF::name() {
	return "day08";
}

std::string SIF::part_01() {
	Value width = 25;
	Value height = 6;
	layers = get_image_as_layers(source, width * height);
	auto layer = get_layer_with_fewest_zeros(layers);
	auto ones = get_count_of_digit_in_layer(*layer, 1);
	auto twos = get_count_of_digit_in_layer(*layer, 2);
	return std::to_string(ones * twos);
}

Layers SIF::get_image_as_layers(const std::string& source, uint64_t size) {
	Layer layer;
	Layers layers;
	for (const auto& ch : source) {
		if (layer.size() >= size) {
			layers.push_back(layer);
			layer.clear();
		}
		layer.push_back(ch - 0x30);
	}
	layers.push_back(layer);
	return layers;
}

Layers::const_iterator SIF::get_layer_with_fewest_zeros(const Layers& layers) {
	Value lowest_zero_count = std::numeric_limits<Value>::max();
	auto result = layers.begin();
	for (auto layer = layers.begin(); layer != layers.end(); layer++) {
		auto count = get_count_of_digit_in_layer(*layer, 0);
		if (count <= lowest_zero_count) {
			lowest_zero_count = count;
			result = layer;
		}
	}
	return result;
}

Value SIF::get_count_of_digit_in_layer(const Layer& layer, Value value) {
	return static_cast<Value>(std::count(layer.begin(), layer.end(), value));
}

std::string SIF::part_02() {
	Layer lay(layers[0].size());
	for (auto lr = layers.crbegin(); lr != layers.crend(); lr++) {
		int idx = 0;
		for (const auto& v : *lr) {
			if (v != 2) {
				lay[idx] = v;
			}
			idx++;
		}
	}
	print_layer(lay, 25);
	return std::string();
}

void SIF::print_layer(const Layer& layer, Value width) {
	Value current_width = 0;
	for (const auto& ch : layer) {
		if (current_width >= width) {
			std::cout << std::endl;
			current_width = 0;
		}
		current_width++;
		switch (ch) {
		case 0:
			std::cout << ' ';
			break;
		case 1:
			std::cout << 'O';
			break;
		case 2:
		default:
			break;
		}
	}
}
