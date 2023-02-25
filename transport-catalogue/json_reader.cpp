#include "json_reader.h"

namespace json_reader {
    Json_TC::Json_TC(std::istream& input)
        : data_(json::Load(input))
    {
    }

    void Json_TC::LoadCatalogueData(transport_catalogue::TransportCatalogue& catalogue) {
        // Загружаем данные в каталог, если они есть
        if (data_.GetRoot().AsMap().count("base_requests"s) > 0) {
            auto& base_requests = data_.GetRoot().AsMap().at("base_requests"s).AsArray();
            // Загружаем данные по остановкам, при вызове AsArray идет проверка на верный формат
            for (auto& request : base_requests) {
                const auto& map = request.AsMap();
                if (map.find("type"s) == map.end()) {
                    throw std::invalid_argument("Invalid request entered"s);
                }
                if (map.at("type"s).AsString() == "Stop"s) {
                    if (!CheckStopRequest(map)) {
                        throw std::invalid_argument("Invalid request entered"s);
                    }
                    LoadStops(catalogue, map);
                }
            }
            for (auto& request : base_requests) {
                const auto& map = request.AsMap();
                if (map.at("type"s).AsString() == "Stop"s && (map.find("road_distances"s) != map.end())) {
                    LoadDistances(catalogue, map);
                }
            }
            for (auto& request : base_requests) {
                const auto& map = request.AsMap();
                if (map.at("type"s).AsString() == "Bus"s) {
                    if (!CheckBusRequest(map)) {
                        throw std::invalid_argument("Invalid request entered"s);
                    }
                    LoadBuses(catalogue, map);
                }
            }
        }
    }

    void Json_TC::LoadStops(transport_catalogue::TransportCatalogue& catalogue, const Dict& stops_data) {
        std::string name = stops_data.at("name"s).AsString();
        geo::Coordinates coord = { stops_data.at("latitude"s).AsDouble(), stops_data.at("longitude"s).AsDouble() };
        catalogue.AddStop(std::move(name), coord);
    }

    void Json_TC::LoadBuses(transport_catalogue::TransportCatalogue& catalogue, const Dict& buses_data) {
        std::string name = buses_data.at("name"s).AsString();
        bool round = buses_data.at("is_roundtrip"s).AsBool();
        std::vector<std::string_view> stops;
        for (const auto& stop : buses_data.at("stops"s).AsArray()) {
            stops.push_back(stop.AsString());
        }
        catalogue.AddBus(std::move(name), stops, round);
    }

    void Json_TC::LoadDistances(transport_catalogue::TransportCatalogue& catalogue, const Dict& stops_data) {
        std::string name = stops_data.at("name"s).AsString();
        for (const auto& stop_dist : stops_data.at("road_distances"s).AsMap()) {
            catalogue.SetDistance({ name, stop_dist.first }, stop_dist.second.AsInt());
        }
    }

    bool Json_TC::CheckStopRequest(const Dict& map_data)const {
        const auto it_end = map_data.end();
        auto it = map_data.find("name"s);
        if (it == it_end && !(it->second.IsString())) {
            return false;
        }
        it = map_data.find("latitude"s);
        if (it == it_end && !(it->second.IsDouble())) {
            return false;
        }
        it = map_data.find("longitude"s);
        if (it == it_end && !(it->second.IsDouble())) {
            return false;
        }
        it = map_data.find("road_distances"s);
        if (!(it == it_end)) {
            return CheckDistanceRequest(it->second.AsMap());
        }
        return true;
    }

    bool Json_TC::CheckDistanceRequest(const Dict& map_data)const {
        for (const auto& dist : map_data) {
            if (!dist.second.IsInt()) {
                return false;
            }
        }
        return true;
    }

    bool Json_TC::CheckBusRequest(const Dict& map_data)const {
        const auto it_end = map_data.end();
        auto it = map_data.find("name"s);
        if (it == it_end && !(it->second.IsString())) {
            return false;
        }
        it = map_data.find("stops"s);
        if (it == it_end && !(it->second.IsArray())) {
            return false;
        }
        for (const auto& stop : it->second.AsArray()) {
            if (!stop.IsString()) {
                return false;
            }
        }
        it = map_data.find("is_roundtrip"s);
        if (it == it_end && !(it->second.IsBool())) {
            return false;
        }
        return true;
    }

    bool Json_TC::CheckStatRequest(const Dict& map_data)const {
        const auto it_end = map_data.end();
        auto it = map_data.find("id"s);
        if (it == it_end && !(it->second.IsInt())) {
            return false;
        }
        it = map_data.find("type"s);
        if (it == it_end && !(it->second.IsString())) {
            return false;
        }
        if (it->second.AsString() == "Stop" || it->second.AsString() == "Bus") {
            auto iter = map_data.find("name"s);
            if (iter == it_end && !(iter->second.IsString())) {
                return false;
            }
        }
        return true;
    }

    svg::Point SetOffset(const Array& offset_data) {
        svg::Point result;
        result.x = offset_data.at(0).AsDouble();
        result.y = offset_data.at(1).AsDouble();
        return result;
    }

    svg::Color SetColor(const Node& color_data) {
        if (color_data.IsString()) {
            return color_data.AsString();
        }
        else if (color_data.IsArray() && color_data.AsArray().size() == 3) {
            auto result_color = svg::Rgb(static_cast<uint8_t>(color_data.AsArray().at(0).AsInt()),
                static_cast<uint8_t>(color_data.AsArray().at(1).AsInt()),
                static_cast<uint8_t>(color_data.AsArray().at(2).AsInt()));
            return result_color;
        }
        else if (color_data.IsArray() && color_data.AsArray().size() == 4) {
            auto result_color = svg::Rgba(static_cast<uint8_t>(color_data.AsArray().at(0).AsInt()),
                static_cast<uint8_t>(color_data.AsArray().at(1).AsInt()),
                static_cast<uint8_t>(color_data.AsArray().at(2).AsInt()),
                color_data.AsArray().at(3).AsDouble());
            return result_color;
        }
        else {
            return svg::NoneColor;
        }
    }

    void Json_TC::LoadRenderSetings(renderer::MapRenderer& render) {
        if (data_.GetRoot().AsMap().count("render_settings"s) > 0) {
            auto& render_settings = data_.GetRoot().AsMap().at("render_settings"s).AsMap();
            renderer::RenderSettings result;
            auto it_end = render_settings.end();
            auto it = render_settings.find("width");
            // Если параметр отсутствует, то оставляем значение по умолчанию
            if (it != it_end) {
                result.size.x = it->second.AsDouble();
            }
            it = render_settings.find("height"s);
            if (it != it_end) {
                result.size.y = it->second.AsDouble();
            }
            it = render_settings.find("padding"s);
            if (it != it_end) {
                result.padding = it->second.AsDouble();
            }
            it = render_settings.find("line_width"s);
            if (it != it_end) {
                result.line_width = it->second.AsDouble();
            }
            it = render_settings.find("stop_radius"s);
            if (it != it_end) {
                result.stop_radius = it->second.AsDouble();
            }
            it = render_settings.find("bus_label_font_size"s);
            if (it != it_end) {
                result.bus_label_font_size = it->second.AsInt();
            }
            it = render_settings.find("bus_label_offset"s);
            if (it != it_end) {
                result.bus_label_offset = SetOffset(it->second.AsArray());
            }
            it = render_settings.find("stop_label_font_size"s);
            if (it != it_end) {
                result.stop_label_font_size = it->second.AsInt();
            }
            it = render_settings.find("stop_label_offset"s);
            if (it != it_end) {
                result.stop_label_offset = SetOffset(it->second.AsArray());
            }
            it = render_settings.find("underlayer_color"s);
            if (it != it_end) {
                result.underlayer_color = SetColor(it->second);
            }
            it = render_settings.find("underlayer_width"s);
            if (it != it_end) {
                result.underlayer_width = it->second.AsDouble();
            }
            it = render_settings.find("color_palette"s);
            if (it != it_end) {
                for (const auto& color : it->second.AsArray()) {
                    result.color_palette.push_back(SetColor(color));
                }
            }
            render.SetRenderSettings(result);
        }
    }

    namespace {
        Dict GetStopForPrint(TransportCatalogueHandler& catalogue, const Dict& stops_data) {
            Dict result;
            Array stops_result;
            const auto& stops = catalogue.GetBusesByStop(stops_data.at("name").AsString());
            if (!stops) {
                result["request_id"s] = stops_data.at("id"s).AsInt();
                result["error_message"s] = "not found"s;
            }
            else {
                for (const auto& stop : *stops) {
                    stops_result.push_back(std::string(stop));
                }
                result["buses"s] = stops_result;
                result["request_id"s] = stops_data.at("id"s).AsInt();
            }
            return result;
        }

        Dict GetBusForPrint(TransportCatalogueHandler& catalogue, const Dict& bus_data) {
            Dict result;
            const auto& bus_info = catalogue.GetBusInfo(bus_data.at("name").AsString());
            if (!bus_info.has_value()) {
                result["request_id"s] = bus_data.at("id"s).AsInt();
                result["error_message"s] = "not found"s;
            }
            else {
                result["curvature"s] = bus_info->actual_length_ / bus_info->geographical_length_;
                result["request_id"s] = bus_data.at("id"s).AsInt();
                result["route_length"s] = bus_info->actual_length_;
                result["stop_count"s] = bus_info->stop_number_;
                result["unique_stop_count"s] = bus_info->unique_stop_;
            }
            return result;
        }

        Dict GetMapForPrint(TransportCatalogueHandler& catalogue, const Dict& map_data) {
            Dict result;
            std::ostringstream out;
            catalogue.RenderMap().Render(out);
            result["map"s] = std::move(out.str());
            result["request_id"s] = map_data.at("id"s).AsInt();
            return result;
        }

    } //namespace

    void Json_TC::PrintCatalogueStatRequests(TransportCatalogueHandler& catalogue, std::ostream& output) const {
        if (data_.GetRoot().AsMap().count("stat_requests"s) > 0) {
            auto& stat_requests = data_.GetRoot().AsMap().at("stat_requests"s).AsArray();
            // Загружаем данные по остановкам, при вызове AsArray идет проверка на верный формат
            Array result;
            for (auto& request : stat_requests) {
                const auto& map = request.AsMap();
                if (!CheckStatRequest(map)) {
                    throw std::invalid_argument("Invalid request entered"s);
                }
                if (map.at("type"s).AsString() == "Stop"s) {
                    result.push_back(GetStopForPrint(catalogue, map));
                }
                if (map.at("type"s).AsString() == "Bus"s) {
                    result.push_back(GetBusForPrint(catalogue, map));
                }
                if (map.at("type"s).AsString() == "Map"s) {
                    result.push_back(GetMapForPrint(catalogue, map));
                }
            }
            Print(Document(result), output);
        }
    }
} //namespace json_reader