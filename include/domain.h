#pragma once

#include <string>
#include <string_view>
#include <set>
#include <vector>

#include "geo.h"


struct Stop {
	std::string name_;
	geo::Coordinates coordinates_;
};

struct Bus {
	std::string name_;
	std::vector<const Stop*> stops_;
	bool round_;
};

struct BusInfo {
	std::string_view name_;
	int stop_number_;
	int unique_stop_;
	double geographical_length_;
	double actual_length_;
};