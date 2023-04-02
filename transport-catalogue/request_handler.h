#pragma once

#include <map>

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

class TransportCatalogueHandler {
public:

    TransportCatalogueHandler(const transport_catalogue::TransportCatalogue& catalogue, const renderer::MapRenderer& renderer
        , const transport_router::TransportRouter& router)
        : catalogue_(catalogue)
        , renderer_(renderer)
        , router_(router)
    {
    }

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusInfo> GetBusInfo(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    const std::set<std::string_view>* GetBusesByStop(const std::string_view& stop_name) const;

    const std::unordered_map<std::string_view, const Bus*>& GetBuses() const;

    const std::unordered_map<std::string_view, const Stop*>& GetStops() const;

    std::optional <graph::Router<transport_router::RouteWeight>::RouteInfo> GetRouter (const std::string_view stop_name_from, const std::string_view stop_name_to) const;

    const graph::DirectedWeightedGraph<transport_router::RouteWeight>& GetGraph() const;

    const transport_router::RouterSettings& GetRouterSettings() const;

    const std::string_view GetStopNameFromID(size_t id) const;

    // Этот метод будет нужен в следующей части итогового проекта
    svg::Document RenderMap() const;

private:
    const transport_catalogue::TransportCatalogue& catalogue_;
    const renderer::MapRenderer& renderer_;
    const transport_router::TransportRouter& router_;
};