#pragma once

#include <string>
#include <deque>
#include <unordered_map>
#include <string_view>
#include <vector>
#include <iterator>
#include <utility>
#include <cassert>
#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <set>

#include "geo.h"

using namespace std::literals;

struct Stop {
	std::string name_;
	Coordinates coordinates_;
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

struct StopInfo {
	std::string_view name_;
	std::set<std::string_view> buses_;
};

struct StopsHasher {

	size_t operator()(const std::pair<const Stop*, const Stop*> stops) const;

private:
	std::hash<std::string_view> hasher;
};

class TransportCatalogue {
public:

	const Bus* FindBus(const std::string_view name) const;
	const Stop* FindStop(const std::string_view name) const;

	void AddStop(std::string name, Coordinates coordinates);
	void AddBus(std::string name, std::vector<std::string> stop_names, bool round);
	void SetDistance(std::pair<std::string_view, std::string_view> from_stop_to_stop, int distance);

	BusInfo GetBusInfo(const std::string_view name) const;
	StopInfo GetStopInfo(const std::string_view name) const;

private:

	std::deque<Stop> stops_;
	std::deque<Bus> buses_;

	std::unordered_map<std::string_view, const Stop*> stopname_to_stop_;
	std::unordered_map<std::string_view, const Bus*> busname_to_bus_;
	std::unordered_map<std::string_view, std::set<std::string_view>> stopname_to_busses_;
	std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopsHasher> stops_distance_;

	int GetNumberOfStops(const std::string_view name) const;
	int GetNumberOfUniqueStops(const std::string_view name) const;

	double GetStopsDistance(const std::pair<const Stop*, const Stop*>) const;
	double GetGeographicalDistance(const std::string_view name) const;
	double GetActualDistance(const std::string_view name) const;
};