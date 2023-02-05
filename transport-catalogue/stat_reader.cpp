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

	void GetInfo(const TransportCatalogue& catalogue, Queue&& queue) {
		for (auto& q : queue) {
			if (q.type == "Bus"s) {
				if (!catalogue.FindBus(q.text)) {
					std::cout << "Bus "s << q.text << ": not found"s << std::endl;
				}
				else {
					BusInfo info = catalogue.GetBusInfo(q.text);
					std::cout << "Bus "s << info.name_ << ": "s << info.stop_number_ << " stops on route, "s << info.unique_stop_ << " unique stops, "s << std::setprecision(6) << info.actual_length_ << " route length, "s << info.actual_length_ / info.geographical_length_ << " curvature" << std::endl;
				}
			}
			else if (q.type == "Stop"s) {
				if (!catalogue.FindStop(q.text)) {
					std::cout << "Stop "s << q.text << ": not found"s << std::endl;
				}
				else {
					auto info = catalogue.GetStopInfo(q.text);
					if (info.buses_.empty()) {
						std::cout << "Stop "s << info.name_ << ": no buses"s << std::endl;
					}
					else {
						std::cout << "Stop "s << info.name_ << ": buses"s;
						for (const auto& bus : info.buses_) {
							std::cout << ' ' << bus;
						}
						std::cout << std::endl;
					}
				}
			}
		}
	}
}