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
		std::vector<Query> queue_;
	};

	void GetInfo(const TransportCatalogue& catalogue, Queue&& queue);
}