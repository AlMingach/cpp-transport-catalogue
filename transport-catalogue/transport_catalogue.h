#pragma once

#include <deque>
#include <unordered_map>
#include <vector>
#include <iterator>
#include <utility>
#include <cassert>
#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <optional>

#include "domain.h"

using namespace std::literals;

struct StopsHasher {

	size_t operator()(const std::pair<const Stop*, const Stop*> stops) const;

private:
	std::hash<std::string_view> hasher;
};

namespace transport_catalogue {

	class TransportCatalogue {
	public:

		const Bus* FindBus(const std::string_view name) const;
		const Stop* FindStop(const std::string_view name) const;

		void AddStop(std::string name, geo::Coordinates coordinates);
		void AddBus(std::string name, std::vector<std::string_view> stop_names, bool round);
		void SetDistance(std::pair<std::string_view, std::string_view> from_stop_to_stop, int distance);

		std::optional<BusInfo> GetBusInfo(const std::string_view name) const;
		const std::set<std::string_view>* GetBusesByStop(const std::string_view name) const;

		const std::unordered_map<std::string_view, const Bus*>& GetBuses() const;
		const std::unordered_map<std::string_view, const Stop*>& GetStops() const;

		double GetStopsDistance(const std::pair<const Stop*, const Stop*>) const;

	private:

		std::deque<Stop> stops_;
		std::deque<Bus> buses_;

		std::unordered_map<std::string_view, const Stop*> stopname_to_stop_;
		std::unordered_map<std::string_view, const Bus*> busname_to_bus_;
		std::unordered_map<std::string_view, std::set<std::string_view>> stopname_to_busses_;
		std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopsHasher> stops_distance_;

		bool IsValueName(std::string_view text);
		bool IsLatitude(double number);
		bool IsLongitude(double number);

		int GetNumberOfStops(const std::string_view name) const;
		int GetNumberOfUniqueStops(const std::string_view name) const;

		double GetGeographicalDistance(const std::string_view name) const;
		double GetActualDistance(const std::string_view name) const;

	};
}