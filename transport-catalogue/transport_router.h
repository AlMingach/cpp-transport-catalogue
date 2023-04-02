#pragma once
#include "transport_catalogue.h"
#include "router.h"

#include <memory>

namespace transport_router {

	// коэффициент перевода км/ч в м/мин
	constexpr static double K_MH_TO_M_MIN = 1000.0 / 60.0;

	struct RouteWeight {
		std::string_view bus_name;
		double total_time = 0;
		int span_count = 0;
	};

	struct RouterSettings {
		double bus_wait_time_;
		double bus_velocity_;
	};

	class TransportRouter {
	public:

		explicit TransportRouter(const transport_catalogue::TransportCatalogue& catalogue_, RouterSettings settings);

		std::optional <graph::Router<RouteWeight>::RouteInfo> GetRouter(const std::string_view stop_name_from, const std::string_view stop_name_to) const;

		const graph::DirectedWeightedGraph<RouteWeight>& GetGraph() const;

		const RouterSettings& GetRouterSettings() const;

		const std::string_view GetStopNameFromID(size_t id) const;

	private:
		RouterSettings settings_;

		std::unordered_map<std::string_view, size_t> stopname_id_;

		std::unordered_map<size_t, std::string_view> id_stopname_;

		std::unique_ptr<graph::Router<RouteWeight>> router_;

		graph::DirectedWeightedGraph<RouteWeight> graph_;

		void AddRouterSetting(RouterSettings settings);

		void InicializeGraph(const transport_catalogue::TransportCatalogue& catalogue_);

		bool CheckArgument(double arg);

		size_t CountStops(const transport_catalogue::TransportCatalogue& catalogue_);

		double ComputeRouteTime(const transport_catalogue::TransportCatalogue& catalogue_, const Stop* stop_from_index, const Stop* stop_to_index);

		void BuildGraph(graph::DirectedWeightedGraph<RouteWeight>& graph, const transport_catalogue::TransportCatalogue& catalogue_,
			const std::vector<const Stop*>& stops, const std::string_view bus_name);
	};

	bool operator<(const RouteWeight& left, const RouteWeight& right);
	bool operator>(const RouteWeight& left, const RouteWeight& right);
	RouteWeight operator+(const RouteWeight& left, const RouteWeight& right);
}