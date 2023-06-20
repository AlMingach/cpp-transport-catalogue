#include "json_reader.h"

namespace json_reader {

    Json_TC::Json_TC(std::istream& input)
        : data_(json::Load(input))
    {
    }

    void Json_TC::LoadCatalogueData(transport_catalogue::TransportCatalogue& catalogue) {
        // Загружаем данные в каталог, если они есть
        if (data_.GetRoot().AsDict().count("base_requests"s) > 0) {
            auto& base_requests = data_.GetRoot().AsDict().at("base_requests"s).AsArray();
            // Загружаем данные по остановкам, при вызове AsArray идет проверка на верный формат
            for (auto& request : base_requests) {
                const auto& map = request.AsDict();
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
                const auto& map = request.AsDict();
                if (map.at("type"s).AsString() == "Stop"s && (map.find("road_distances"s) != map.end())) {
                    LoadDistances(catalogue, map);
                }
            }
            for (auto& request : base_requests) {
                const auto& map = request.AsDict();
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
        for (const auto& stop_dist : stops_data.at("road_distances"s).AsDict()) {
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
            return CheckDistanceRequest(it->second.AsDict());
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
        if (data_.GetRoot().AsDict().count("render_settings"s) > 0) {
            auto& render_settings = data_.GetRoot().AsDict().at("render_settings"s).AsDict();
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

    transport_router::RouterSettings Json_TC::GetRouterSettings() const {
        auto& render_settings = data_.GetRoot().AsDict().at("routing_settings"s).AsDict();
        double time = 0;
        double velocity = 0;
        auto it_end = render_settings.end();
        auto it = render_settings.find("bus_wait_time");
        if (it != it_end) {
            time = it->second.AsInt();
        }
        it = render_settings.find("bus_velocity");
        if (it != it_end) {
            velocity = it->second.AsInt();
        }
        return { time, velocity };
    }

    serialize::SerializeSettings Json_TC::GetSerializeSettings() const {
        auto& serialize_settings = data_.GetRoot().AsDict().at("serialization_settings"s).AsDict();
        auto it_end = serialize_settings.end();
        auto it = serialize_settings.find("file");
        return {it->second.AsString()};
    }

    namespace {

        Node GetErrorMassage(int id) {
            Builder result;
            return result.StartDict().Key("request_id"s).Value(id)
                .Key("error_message"s).Value("not found"s).EndDict().Build();
        }

        Node GetStopForPrint(const TransportCatalogueHandler& catalogue, const Dict& stops_data) {
            Builder result;
            Array stops_result;
            const auto& stops = catalogue.GetBusesByStop(stops_data.at("name").AsString());
            if (!stops) {
                result.Value(GetErrorMassage(stops_data.at("id"s).AsInt()));
            }
            else {
                for (const auto& stop : *stops) {
                    stops_result.push_back(std::string(stop));
                }
                result.StartDict().Key("buses"s).Value(stops_result)
                    .Key("request_id"s).Value(stops_data.at("id"s).AsInt()).EndDict();
            }
            return result.Build();
        }

        Node GetBusForPrint(const TransportCatalogueHandler& catalogue, const Dict& bus_data) {
            Builder result;
            const auto& bus_info = catalogue.GetBusInfo(bus_data.at("name").AsString());
            if (!bus_info.has_value()) {
                result.Value(GetErrorMassage(bus_data.at("id"s).AsInt()));
            }
            else {
                result.StartDict().Key("curvature"s).Value(bus_info->actual_length_ / bus_info->geographical_length_)
                    .Key("request_id"s).Value(bus_data.at("id"s).AsInt())
                    .Key("route_length"s).Value(bus_info->actual_length_)
                    .Key("stop_count"s).Value(bus_info->stop_number_)
                    .Key("unique_stop_count"s).Value(bus_info->unique_stop_).EndDict();
            }
            return result.Build();
        }

        Node GetMapForPrint(const TransportCatalogueHandler& catalogue, const Dict& map_data) {
            std::ostringstream out;
            catalogue.RenderMap().Render(out);
            return Builder{}.StartDict().Key("map"s).Value(std::move(out.str()))
                .Key("request_id"s).Value(map_data.at("id"s).AsInt()).EndDict().Build();
        }

        Node GetRouerForPrint(const TransportCatalogueHandler& catalogue, const Dict& router_data) {
            Builder result;
            std::string_view stop_from = router_data.at("from"s).AsString();
            std::string_view stop_to = router_data.at("to"s).AsString();
            int id = router_data.at("id"s).AsInt();
            auto router = catalogue.BuildRouter(stop_from, stop_to);
            if (!router) {
                result.Value(GetErrorMassage(id));
            }
            else {
                const auto& graph = catalogue.GetGraph();
                result.StartDict().Key("items"s).StartArray();
                for (const auto& edge : router->edges) {
                    const auto& edge_info = graph.GetEdge(edge);
                    auto wait_time = catalogue.GetRouterSettings().bus_wait_time_;
                    result.StartDict().Key("stop_name"s).Value(std::string(catalogue.GetStopNameFromID(edge_info.from)))
                        .Key("time"s).Value(wait_time)
                        .Key("type"s).Value("Wait"s).EndDict()
                        .StartDict().Key("bus"s).Value(std::string(edge_info.weight.bus_name))
                        .Key("span_count"s).Value(edge_info.weight.span_count)
                        .Key("time"s).Value(edge_info.weight.total_time - wait_time)
                        .Key("type"s).Value("Bus"s).EndDict();
                }
                result.EndArray().Key("request_id"s).Value(id)
                    .Key("total_time"s).Value(router->weight.total_time).EndDict();
            }
            return result.Build();
        }

    } //namespace

    void Json_TC::PrintCatalogueStatRequests(const TransportCatalogueHandler& catalogue, std::ostream& output) {
        if (data_.GetRoot().AsDict().count("stat_requests"s) > 0) {
            auto& stat_requests = data_.GetRoot().AsDict().at("stat_requests"s).AsArray();
            // Загружаем данные по остановкам, при вызове AsArray идет проверка на верный формат
            Builder result;
            result.StartArray();
            for (auto& request : stat_requests) {
                const auto& map = request.AsDict();
                if (!CheckStatRequest(map)) {
                    throw std::invalid_argument("Invalid request entered"s);
                }
                if (map.at("type"s).AsString() == "Stop"s) {
                    result.Value(GetStopForPrint(catalogue, map));
                }
                if (map.at("type"s).AsString() == "Bus"s) {
                    result.Value(GetBusForPrint(catalogue, map));
                }
                if (map.at("type"s).AsString() == "Map"s) {
                    result.Value(GetMapForPrint(catalogue, map));
                }
                if (map.at("type"s).AsString() == "Route"s) {
                    result.Value(GetRouerForPrint(catalogue, map));
                }
            }
            Print(Document(result.EndArray().Build()), output);
        }
    }
} //namespace json_reader