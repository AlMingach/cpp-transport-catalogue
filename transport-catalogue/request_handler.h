#pragma once

#include <map>

#include "transport_catalogue.h"
#include "map_renderer.h"

class TransportCatalogueHandler {
public:

    TransportCatalogueHandler(transport_catalogue::TransportCatalogue& catalogue, renderer::MapRenderer& renderer)
        : catalogue_(catalogue)
        , renderer_(renderer)
    {
    }

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusInfo> GetBusInfo(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    const std::set<std::string_view>* GetBusesByStop(const std::string_view& stop_name) const;

    const std::unordered_map<std::string_view, const Bus*>& GetBuses() const;

    const std::unordered_map<std::string_view, const Stop*>& GetStops() const;

    // Этот метод будет нужен в следующей части итогового проекта
    svg::Document RenderMap() const;

private:
    const transport_catalogue::TransportCatalogue& catalogue_;
    const renderer::MapRenderer renderer_;
};