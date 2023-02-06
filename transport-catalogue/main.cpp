
#include "input_reader.h"
#include "stat_reader.h"


int main() {

	TransportCatalogue catalogue;
	load_queue::Queue queue_load(std::cin);

	load_queue::LoadInfo(catalogue, std::move(queue_load));

	get_queue::Queue queue_get(std::cin);

	get_queue::GetInfo(catalogue, std::move(queue_get), std::cout);

}