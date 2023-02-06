#pragma once
#include "transport_catalogue.h"

#include <iomanip>

namespace get_queue {

	struct Query {
		std::string type;
		std::string text;
	};

	class Queue {
	public:
		Queue(std::istream& input);

		std::vector<Query>::iterator begin();

		std::vector<Query>::iterator end();

	private:
		std::vector<Query> queue_; //Без формирования очереди запроса на вывод вывод становится неккоректным для тренажера
	};

	void GetBusInfo(const TransportCatalogue& catalogue, std::string bus_name, std::ostream& out);

	void GetStopInfo(const TransportCatalogue& catalogue, std::string stop_name, std::ostream& out);

	void GetInfo(const TransportCatalogue& catalogue, Queue&& queue, std::ostream& out);
}