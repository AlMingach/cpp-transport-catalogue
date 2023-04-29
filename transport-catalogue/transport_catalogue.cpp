#include "transport_catalogue.h"

size_t StopsHasher::operator()(const std::pair<const Stop*, const Stop*> stops) const {
	return hasher(stops.first->name_) + 37 * hasher(stops.second->name_);
}

namespace transport_catalogue {

	void TransportCatalogue::AddStop(std::string name, geo::Coordinates coordinates) {
		if (!IsLatitude(coordinates.lat)) {
			throw std::invalid_argument("latitude > 90.0 or < -90.0"s);
		}
		if (!IsLongitude(coordinates.lng)) {
			throw std::invalid_argument("longitude > 180.0 or < -180.0"s);
		}
		//if (!IsValueName(name)) {
		//	throw std::invalid_argument("Stop name contains prohibited characters"s);
		//}
		Stop st = { std::move(name), coordinates };
		Stop& stop = stops_.emplace_back(std::move(st));
		stopname_to_stop_[stop.name_] = &stop;
		stopname_to_busses_[stop.name_];
	}

	void TransportCatalogue::AddBus(std::string name, std::vector<std::string_view> stop_names, bool round) {
		//if (!IsValueName(name)) {
		//	throw std::invalid_argument("Bus name contains prohibited characters"s);
		//}
		std::vector<const Stop*> stops;
		for (auto& stop : stop_names) {
			const auto& st = FindStop(stop);
			if (!st) {
				throw std::out_of_range("The stop is not in the catalog"s);
			}
			stops.push_back(st);
		}
		Bus bus = { std::move(name), std::move(stops), round };
		Bus& bus1 = buses_.emplace_back(std::move(bus));
		busname_to_bus_[bus1.name_] = &bus1;
		for (const auto& stop : bus1.stops_) {
			stopname_to_busses_[stop->name_].insert(bus1.name_);
		}
	}

	void TransportCatalogue::SetDistance(std::pair<std::string_view, std::string_view> from_stop_to_stop, int distance) {
		const Stop* from_stop = FindStop(from_stop_to_stop.first);
		const Stop* to_stop = FindStop(from_stop_to_stop.second);
		if (!to_stop) {
			throw std::out_of_range("The stop is not in the catalog"s);
		}
		stops_distance_[{from_stop, to_stop}] = distance;
	}

	const Bus* TransportCatalogue::FindBus(const std::string_view name) const {
		return (busname_to_bus_.find(name) != busname_to_bus_.end()) ? busname_to_bus_.at(name) : nullptr;
	}

	const Stop* TransportCatalogue::FindStop(const std::string_view name) const {
		return (stopname_to_stop_.find(name) != stopname_to_stop_.end()) ? stopname_to_stop_.at(name) : nullptr;
	}

	int TransportCatalogue::GetNumberOfStops(const std::string_view name) const {
		const auto bus = FindBus(name);
		return bus->round_ ? bus->stops_.size() : bus->stops_.size() * 2 - 1;
	}

	int TransportCatalogue::GetNumberOfUniqueStops(const std::string_view name) const {
		const auto bus = FindBus(name);
		std::unordered_set<std::string_view> result;
		for (const auto& stop : bus->stops_) {
			result.insert(stop->name_);
		}
		return result.size();
	}

	double TransportCatalogue::GetStopsDistance(const std::pair<const Stop*, const Stop*> stops) const {
		auto it = stops_distance_.find(stops);
		it = (it == stops_distance_.end()) ? stops_distance_.find({ stops.second, stops.first }) : it;
		if (it != stops_distance_.end()) {
			return it->second;
		}
		else {
			return ComputeDistance(stops.first->coordinates_, stops.first->coordinates_);
		}
	}

	double TransportCatalogue::GetGeographicalDistance(const std::string_view name) const {
		const auto bus = FindBus(name);
		double result = 0.0;
		for (auto iter_1 = bus->stops_.begin(), iter_2 = iter_1 + 1; iter_2 < bus->stops_.end(); ++iter_1, ++iter_2) {
			result += ComputeDistance((*iter_1)->coordinates_, (*iter_2)->coordinates_);
		}
		return bus->round_ ? result : (2 * result);
	}

	double TransportCatalogue::GetActualDistance(const std::string_view name) const {
		const auto bus = FindBus(name);
		double result = 0.0;
		for (auto iter_1 = bus->stops_.begin(), iter_2 = iter_1 + 1; iter_2 < bus->stops_.end(); ++iter_1, ++iter_2) {
			result += GetStopsDistance({ *iter_1, *iter_2 });
		}
		if (!bus->round_) {
			for (auto iter_1 = bus->stops_.rbegin(), iter_2 = iter_1 + 1; iter_2 < bus->stops_.rend(); ++iter_1, ++iter_2) {
				result += GetStopsDistance({ *iter_1, *iter_2 });
			}
		}
		return result;
	}

	std::optional<BusInfo> TransportCatalogue::GetBusInfo(const std::string_view name) const {
		const auto bus = FindBus(name);
		if (!bus) {
			return std::nullopt;
		}
		return BusInfo{ bus->name_, GetNumberOfStops(bus->name_), GetNumberOfUniqueStops(bus->name_), GetGeographicalDistance(bus->name_), GetActualDistance(bus->name_) };
	}

	const std::set<std::string_view>* TransportCatalogue::GetBusesByStop(const std::string_view name) const {
		auto it = stopname_to_busses_.find(name);
		if (it == stopname_to_busses_.end()) {
			return nullptr;
		}
		return &it->second;
	}

	//bool TransportCatalogue::IsValueName(std::string_view text) {
	//	return 	std::any_of(text.begin(), text.end(), [](char c) {
	//		return isalnum(c);
	//		});;
	//}

	bool TransportCatalogue::IsLatitude(double number) {
		return -90.0 <= number && number <= 90.0;
	}

	bool TransportCatalogue::IsLongitude(double number) {
		return -180.0 <= number && number <= 180.0;
	}

	const std::unordered_map<std::string_view, const Bus*>& TransportCatalogue::GetBuses() const {
		return busname_to_bus_;
	}

	const std::unordered_map<std::string_view, const Stop*>& TransportCatalogue::GetStops() const {
		return stopname_to_stop_;
	}

	const std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopsHasher> TransportCatalogue::GetSetDistances() const {
		return stops_distance_;
	}
}