#pragma once

#include <map>

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"
#include "serialization.h"

class TransportCatalogueHandler {
public:

    // Конструктор фасада
    TransportCatalogueHandler(const transport_catalogue::TransportCatalogue& catalogue, const renderer::MapRenderer& renderer
        , const transport_router::TransportRouter& router)
        : catalogue_(catalogue)
        , renderer_(renderer)
        , router_(router)
    {
    }

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusInfo> GetBusInfo(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через остановку
    const std::set<std::string_view>* GetBusesByStop(const std::string_view& stop_name) const;

    // Возвращает все маршруты
    const std::unordered_map<std::string_view, const Bus*>& GetBuses() const;

    // Возвращает все остановки
    const std::unordered_map<std::string_view, const Stop*>& GetStops() const;

    // Построение короткого маршрута
    std::optional <graph::Router<transport_router::RouteWeight>::RouteInfo> BuildRouter (const std::string_view stop_name_from, const std::string_view stop_name_to) const;

    // Получение графа
    const graph::DirectedWeightedGraph<transport_router::RouteWeight>& GetGraph() const;

    // Получение заданный настроек маршрута
    const transport_router::RouterSettings& GetRouterSettings() const;

    // Получение названия маршрута по id
    const std::string_view GetStopNameFromID(size_t id) const;
    
    // Построение карты
    svg::Document RenderMap() const;

private:
    const transport_catalogue::TransportCatalogue& catalogue_;
    const renderer::MapRenderer& renderer_;
    const transport_router::TransportRouter& router_;
};