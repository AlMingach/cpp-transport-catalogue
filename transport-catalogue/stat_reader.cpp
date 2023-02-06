#include "stat_reader.h"

namespace get_queue {

	Queue::Queue(std::istream& input) {
		std::string size;
		std::getline(input, size);
		queue_.reserve(std::stoi(size));

		for (int i = 0; i < stoi(size); ++i) {
			std::string text;
			std::getline(input, text);
			size_t begin = text.find_first_not_of(' ');
			size_t lenght_1 = text.find_first_of(' ', begin);
			std::string queue_type = std::move(text.substr(begin, lenght_1 - begin));
			begin = text.find_first_not_of(' ', lenght_1);
			lenght_1 = text.find_last_not_of(' ');
			std::string name = std::move(text.substr(begin, lenght_1 - begin + 1));
			queue_.push_back({ queue_type , name });
		}
	}

	std::vector<Query>::iterator Queue::begin() {
		return queue_.begin();
	}

	std::vector<Query>::iterator Queue::end() {
		return queue_.end();
	}

	void GetBusInfo(const TransportCatalogue& catalogue, std::string bus_name, std::ostream& out) {
		if (!catalogue.FindBus(bus_name)) {
			out << "Bus "s << bus_name << ": not found"s << std::endl;
		}
		else {
			BusInfo info = catalogue.GetBusInfo(bus_name);
			out << "Bus "s << info.name_ << ": "s << info.stop_number_ << " stops on route, "s << info.unique_stop_ << " unique stops, "s << std::setprecision(6) << info.actual_length_ << " route length, "s << info.actual_length_ / info.geographical_length_ << " curvature" << std::endl;
		}
	}

	void GetStopInfo(const TransportCatalogue& catalogue, std::string stop_name, std::ostream& out) {
		if (!catalogue.FindStop(stop_name)) {
			out << "Stop "s << stop_name << ": not found"s << std::endl;
		}
		else {
			auto info = catalogue.GetStopInfo(stop_name);
			if (info.buses_.empty()) {
				out << "Stop "s << info.name_ << ": no buses"s << std::endl;
			}
			else {
				std::cout << "Stop "s << info.name_ << ": buses"s;
				for (const auto& bus : info.buses_) {
					out << ' ' << bus;
				}
				out << std::endl;
			}
		}
	}

	void GetInfo(const TransportCatalogue& catalogue, Queue&& queue, std::ostream& out) {
		for (auto& q : queue) {
			if (q.type == "Bus"s) {
				GetBusInfo(catalogue, q.text, out);
			}
			else if (q.type == "Stop"s) {
				GetStopInfo(catalogue, q.text, out);
			}
		}
	}
}