#pragma once

#include "json.h"
#include "request_handler.h"
#include "json_builder.h"

#include <sstream>
#include <cctype>

namespace json_reader {

	using namespace json;

	class Json_TC {
	public:
		Json_TC(std::istream& input);

		void LoadCatalogueData(transport_catalogue::TransportCatalogue& catalogue);

		void LoadRenderSetings(renderer::MapRenderer& render);

		void PrintCatalogueStatRequests(TransportCatalogueHandler& catalogue, std::ostream& output) const;


	private:
		Document data_;

		void LoadStops(transport_catalogue::TransportCatalogue& catalogue, const Dict& stops_data);

		void LoadBuses(transport_catalogue::TransportCatalogue& catalogue, const Dict& buses_data);

		void LoadDistances(transport_catalogue::TransportCatalogue& catalogue, const Dict& buses_data);

		bool CheckDistanceRequest(const Dict& map_data)const;

		bool CheckStopRequest(const Dict& map_data)const;

		bool CheckBusRequest(const Dict& map_data)const;

		bool CheckStatRequest(const Dict& map_data)const;

	};
} //namespace json_reader

