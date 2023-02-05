#include "input_reader.h"
#include "stat_reader.h"


int main() {

	TransportCatalogue catalog;
	load_queue::Queue queue_load(std::cin);

	load_queue::LoadInfo(catalog, std::move(queue_load));

	get_queue::Queue queue_get(std::cin);

	get_queue::GetInfo(catalog, std::move(queue_get));

}