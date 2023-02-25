#include "request_handler.h"

 // Возвращает информацию о маршруте (запрос Bus)
std::optional<BusInfo> TransportCatalogueHandler::GetBusInfo(const std::string_view& bus_name) const {
	return catalogue_.GetBusInfo(bus_name);
}

// Возвращает маршруты, проходящие через
const std::set<std::string_view>* TransportCatalogueHandler::GetBusesByStop(const std::string_view& stop_name) const {
	return catalogue_.GetBusesByStop(stop_name);
}

// Возвращает все маршруты из каталога
const std::unordered_map<std::string_view, const Bus*>& TransportCatalogueHandler::GetBuses() const {
    return catalogue_.GetBuses();
}

// Возвращает все остановки из каталога
const std::unordered_map<std::string_view, const Stop*>& TransportCatalogueHandler::GetStops() const {
    return catalogue_.GetStops();
}

// Возвращает карту маршрутов
svg::Document TransportCatalogueHandler::RenderMap() const {
    if (!renderer_.GetValue()) {
        return {};
    }
    // Создание отсортированного контейнера остановок, которые учавствуют в маршрутах
    std::map<std::string_view, const Stop*> stops;
    for (const auto& stop : GetStops()) {
        if (!GetBusesByStop(stop.first)->empty()) {
            stops.insert(stop);
        }
    }
    
    // Сортируем маршруты
    std::map<std::string_view, const Bus*> buses;
    for (const auto& bus : GetBuses()) {
        buses.insert(bus);
    }

    return renderer_.RenderMap(buses, stops);
}

