#include "input_reader.h"

namespace load_queue {

	Stop::Stop(std::string name, std::string latitude, std::string longitude) {
		if (!IsValueText(name)) {
			throw std::invalid_argument("stop name contain invalid characters");
		}
		double lat = std::stod(latitude);
		if (!IsLatitude(lat)) {
			throw std::out_of_range("latitude < -90 or latitude > 90");
		}
		double lon = std::stod(longitude);
		if (!Islongitude(lon)) {
			throw std::out_of_range("longitude < -180 or longitude > 180");
		}
		name_ = name;
		coordinates_ = { lat , lon };
	}

	std::string& Stop::GetName() {
		return name_;
	}

	Coordinates& Stop::GetCoordinates() {
		return coordinates_;
	}

	StopDistance::StopDistance(std::string name, std::string text) {
		if (!IsValueText(name)) {
			throw std::invalid_argument("stop name contain invalid characters");
		}
		name_ = name;
		size_t begin = 0;
		while (begin != text.size()) {
			std::string stop_name;
			std::string distance;
			begin = text.find_first_not_of(' ', begin + 1);
			size_t last = text.find_first_of('m', begin);
			distance = std::move(text.substr(begin, last - begin));
			begin = text.find_first_not_of(' ', text.find_first_not_of(' ', last + 1) + 2);
			last = std::min(text.find_first_of(',', begin), text.size());
			stop_name = std::move(text.substr(begin, text.find_last_not_of(' ', last - 1) - begin + 1));
			if (!IsValueText(stop_name)) {
				throw std::invalid_argument("stop name contain invalid characters");
			}
			stops_distance_[stop_name] = std::stoi(distance);
			begin = last;
		}
	}

	std::string& StopDistance::GetName() {
		return name_;
	}

	std::unordered_map<std::string, int>& StopDistance::GetDistanceToStop() {
		return stops_distance_;
	}

	Bus::Bus(std::string name, std::string text) {
		if (!IsValueText(name)) {
			throw std::invalid_argument("stop name contain invalid characters");
		}
		name_ = name;

		size_t size = std::count(text.begin(), text.end(), '>');
		if (size > 0) {
			stops_.reserve(size + 1);
			round_ = true;
			ParseStopName(text, '>');
		}
		else {
			stops_.reserve(size + 1);
			round_ = false;
			ParseStopName(text, '-');
		}
	}

	std::string& Bus::GetName() {
		return name_;
	}

	std::vector<std::string>& Bus::GetStops() {
		return stops_;
	}

	void Bus::ParseStopName(std::string text, char c) {
		size_t begin = 0;
		while (begin != text.size()) {
			std::string stop_name;
			begin = text.find_first_not_of(' ', begin + 1);
			size_t last = std::min(text.find_first_of(c, begin), text.size());
			stop_name = std::move(text.substr(begin, text.find_last_not_of(' ', last - 1) - begin + 1));
			if (!IsValueText(stop_name)) {
				throw std::invalid_argument("stop name contain invalid characters");
			}
			stops_.push_back(stop_name);
			begin = last;
		}
	}


	bool Bus::GetRound() {
		return round_;
	}

	Queue::Queue(std::istream& input) {
		std::string size;
		std::getline(input, size);

		for (int i = 0; i < stoi(size); ++i) {
			std::string text;
			std::getline(input, text);
			size_t begin = text.find_first_not_of(' ');
			size_t lenght_1 = text.find_first_of(' ', begin);
			std::string queue_type = std::move(text.substr(begin, lenght_1 - begin));
			if (queue_type == "Stop") {
				begin = text.find_first_not_of(' ', lenght_1);
				lenght_1 = text.find_first_of(':', begin);

				std::string name = std::move(text.substr(begin, text.find_last_not_of(' ', lenght_1 - 1) - begin + 1));

				begin = text.find_first_not_of(' ', lenght_1 + 1);
				lenght_1 = text.find_first_of(',', begin);

				std::string latitude = std::move(text.substr(begin, text.find_last_not_of(' ', lenght_1 - 1) - begin + 1));

				begin = text.find_first_not_of(' ', lenght_1 + 1);
				lenght_1 = std::min(text.find_first_of(',', begin), text.size());

				std::string longitude = std::move(text.substr(begin, text.find_last_not_of(' ', lenght_1 - 1) - begin + 1));

				stops_.push_back(std::move(Stop(name, latitude, longitude)));

				if (lenght_1 != text.size()) {
					begin = lenght_1;
					lenght_1 = text.find_last_not_of(' ');
					stops_distance_.push_back(std::move(StopDistance(name, std::move(text.substr(begin, lenght_1 - begin + 1)))));
				}
			}
			else {
				begin = text.find_first_not_of(' ', lenght_1);
				lenght_1 = text.find_first_of(':', begin);
				std::string bus_name = std::move(text.substr(begin, text.find_last_not_of(' ', lenght_1 - 1) - begin + 1));
				std::string stops = std::move(text.substr(lenght_1, text.find_last_not_of(' ') + 1));
				buses_.push_back(Bus(bus_name, stops));
			}
		}
	}

	std::vector<Stop>& Queue::GetStops() {
		return stops_;
	}

	std::vector<StopDistance>& Queue::GetStopDistance() {
		return stops_distance_;
	}

	std::vector<Bus>& Queue::GetBus() {
		return buses_;
	}

	bool IsValueText(std::string_view text) {
		return std::any_of(text.begin(), text.end(), [](char c) {
			return (c >= 'A' && c <= 'z') || (c == ' ') || (c >= '0' && c <= '9');
			});
	}

	bool IsLatitude(double number) {
		return -90.0 <= number && number <= 90.0;
	}

	bool Islongitude(double number) {
		return -180.0 <= number && number <= 180.0;
	}

	void LoadInfo(TransportCatalogue& catalogue, Queue&& queue) {
		for (auto& stop : queue.GetStops()) {
			catalogue.AddStop(std::move(stop.GetName()), std::move(stop.GetCoordinates()));
		}
		for (auto& distance : queue.GetStopDistance()) {
			catalogue.AddDistance(distance.GetName(), distance.GetDistanceToStop());
		}
		for (auto& bus : queue.GetBus()) {
			catalogue.AddBus(bus.GetName(), bus.GetStops(), bus.GetRound());
		}
	}

}