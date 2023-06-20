#include "serialization.h"

namespace serialize {

    using ProtoTransportCatalogue = transport_catalogue_serialize::TransportCatalogue;
    using TransportCatalogue = transport_catalogue::TransportCatalogue;
    using ProtoCoordinates = transport_catalogue_serialize::Coordinates;
    using MapRenderer = renderer::MapRenderer;
    using TransportRouter = transport_router::TransportRouter;

    void Serializator::SetSerializeSettings(SerializeSettings settings) {
         settings_ = settings;
     }

    void Serializator::AddTransportCatalogue(const TransportCatalogue& catalogue) {
        SetProtoStops(catalogue, proto_catalogue_);
        SetProtoBuses(catalogue, proto_catalogue_);
        SetProtoDistances(catalogue, proto_catalogue_);
    }

    void Serializator::AddRenderSettings(const MapRenderer& renderer) {
        const auto& render_settings = renderer.GetValue();
        auto proto = proto_catalogue_.mutable_render_settings();

        proto->mutable_size()->set_x(render_settings.size.x);
        proto->mutable_size()->set_y(render_settings.size.y);

        proto->set_padding(render_settings.padding);

        proto->set_line_width(render_settings.line_width);

        proto->set_stop_radius(render_settings.stop_radius);

        proto->set_bus_label_font_size(render_settings.bus_label_font_size);

        proto->mutable_bus_label_offset()->set_x(render_settings.bus_label_offset.x);
        proto->mutable_bus_label_offset()->set_y(render_settings.bus_label_offset.y);

        proto->set_stop_label_font_size(render_settings.stop_label_font_size);

        proto->mutable_stop_label_offset()->set_x(render_settings.stop_label_offset.x);
        proto->mutable_stop_label_offset()->set_y(render_settings.stop_label_offset.y);

        *proto->mutable_underlayer_color() = MakeColorToProto(render_settings.underlayer_color);

        proto->set_underlayer_width(render_settings.underlayer_width);

        for (auto& color : render_settings.color_palette) {
            *proto->add_color_palette() = MakeColorToProto(color);
        }
    }

    void Serializator::SetProtoTransportRouter(const TransportRouter& router) {
        AddTransportRouterSettings(router.GetRouterSettings());
        AddGraph(router.GetGraph());
        AddRouter(router.GetRouter());
    }

    void Serializator::AddTransportRouterSettings(const transport_router::RouterSettings& settings) {
        auto proto_router = proto_catalogue_.mutable_router();
        proto_router->mutable_settings()->set_bus_velocity(settings.bus_velocity_);
        proto_router->mutable_settings()->set_bus_wait_time(settings.bus_wait_time_);
    }

    void Serializator::AddGraph(const graph::DirectedWeightedGraph<transport_router::RouteWeight>& graph) {
        auto proto_router = proto_catalogue_.mutable_router();
        auto graph_size = graph.GetEdgeCount();
        auto proto_graph = proto_router->mutable_graph();
        for (auto& edge : graph.GetEdges()) {
            graph_serialize::Edge proto_edge;
            proto_edge.set_vertex_id_from(edge.from);
            proto_edge.set_vertex_id_to(edge.to);
            proto_edge.mutable_weight()->set_bus_name(bus_name_to_id_.at(edge.weight.bus_name));
            proto_edge.mutable_weight()->set_total_time(edge.weight.total_time);
            proto_edge.mutable_weight()->set_span_count(edge.weight.span_count);
            *proto_graph->add_edges() = std::move(proto_edge);
        }

        for (auto& list : graph.GetIncidenceLists()) {
            auto proto_list = proto_graph->add_incidence_list();
            for (auto id : list) {
                proto_list->add_edges_id(id);
            }
        }
    }

    
    void Serializator::AddRouter(const std::unique_ptr<graph::Router<transport_router::RouteWeight>>& router) {
        auto proto_router = proto_catalogue_.mutable_router()->mutable_router();

        for (const auto& data : router->GetRoutesInternalData()) {
            graph_serialize::RoutesInternalData p_data;
            for (const auto& internal : data) {
                graph_serialize::OptionalRouteInternalData p_internal;
                if (internal.has_value()) {
                    auto& value = internal.value();
                    auto p_value = p_internal.mutable_data();
                    p_value->mutable_route_weight()->set_total_time(value.weight.total_time);
                    if (value.prev_edge.has_value()) {
                        p_value->mutable_prev_edge()->set_edge_id(value.prev_edge.value());
                    }
                }
                *p_data.add_routes_internal_data() = std::move(p_internal);
            }
        *proto_router->add_routes_data() = std::move(p_data);
        }
    }

    void Serializator::SetProtoStops(const TransportCatalogue &catalogue, ProtoTransportCatalogue& proto_catalogue) {
        const auto &stops = catalogue.GetStops();
        uint32_t id = 0;
        for (const auto& [name_, stop] : stops) {
            transport_catalogue_serialize::Stop proto_stop;
            std::string name(name_);
            proto_stop.set_id(id);
            proto_stop.set_name(name);
            stop_name_to_id_.insert({name_, id++});
            *proto_stop.mutable_coordinates() = MakeCoordinatesToProto(stop->coordinates_);
            *proto_catalogue.mutable_catalogue()->add_stops() = std::move(proto_stop);
        }
    }

    ProtoCoordinates Serializator::MakeCoordinatesToProto(const geo::Coordinates &coordinates) {
        ProtoCoordinates proto_coordinates;
        proto_coordinates.set_lat(coordinates.lat);
        proto_coordinates.set_lng(coordinates.lng);
        return proto_coordinates;
    }

    svg_serialize::Color Serializator::MakeColorToProto(const svg::Color& color) {
        svg_serialize::Color result;

        if (std::holds_alternative<std::string>(color)) {

            result.set_string_color(std::get<std::string>(color));

        } else if (std::holds_alternative<svg::Rgb>(color)) {

            auto &rgb = std::get<svg::Rgb>(color);
            auto proto_color = result.mutable_rgb_color();
            proto_color->set_r(rgb.red);
            proto_color->set_g(rgb.green);
            proto_color->set_b(rgb.blue);

        } else if (std::holds_alternative<svg::Rgba>(color)) {

            auto &rgba = std::get<svg::Rgba>(color);
            auto proto_color = result.mutable_rgba_color();
            proto_color->set_r(rgba.red);
            proto_color->set_g(rgba.green);
            proto_color->set_b(rgba.blue);
            proto_color->set_o(rgba.opacity);

        }
        return result;
    }

    void Serializator::SetProtoBuses(const TransportCatalogue &catalogue, ProtoTransportCatalogue& proto_catalogue) {
        const auto& buses = catalogue.GetBuses();
        uint32_t id = 0;
        for (const auto& [name_, bus] : buses) {
            transport_catalogue_serialize::Bus proto_bus;
            std::string name(name_);
            proto_bus.set_id(id);
            proto_bus.set_name(name);
            proto_bus.set_type(bus->round_);

            for (const auto stop : bus->stops_) {
                proto_bus.add_stop_ids(stop_name_to_id_.at(stop->name_));
            }

            bus_name_to_id_.insert({name_, id++});

            *proto_catalogue.mutable_catalogue()->add_buses() = std::move(proto_bus);
        }
    }

    void Serializator::SetProtoDistances(const TransportCatalogue& catalogue, ProtoTransportCatalogue& proto_catalogue) {
        const auto& distances = catalogue.GetSetDistances();
        for (const auto &[stops, distance] : distances) {
            transport_catalogue_serialize::Distance proto_distance;
            proto_distance.set_stop_id_from(stop_name_to_id_.at(stops.first->name_));
            proto_distance.set_stop_id_to(stop_name_to_id_.at(stops.second->name_));
            proto_distance.set_distance(distance);
            *proto_catalogue.mutable_catalogue()->add_distances() = std::move(proto_distance);
        }
    }

    void Serializator::Serialize() const {
        std::ofstream ofs(settings_.path, std::ios::binary);

        if (!ofs.is_open()) {
            std::cerr << "File did not open"sv << '\n'
                << "Serialization failed"sv << std::endl;
            return;
        }

        proto_catalogue_.SerializeToOstream(&ofs);
    }

    void Serializator::Deserialize(TransportCatalogue& catalogue, MapRenderer& renderer, TransportRouter& router) {
        std::ifstream ifs(settings_.path, std::ios::binary);
        if (!ifs.is_open()) {
            std::cerr << "File did not open"sv << '\n'
                << "Deserialization failed"sv << std::endl;
            return;
        }
        if (!proto_catalogue_.ParseFromIstream(&ifs)) {
            std::cerr << "Parse failed"sv << '\n'
                << "Desialization failed"sv << std::endl;
            return;
        }
        SetStopsToCatalogue(catalogue);
        SetBusesToCatalogue(catalogue);
        SetDistancesToCatalogue(catalogue);
        SetRendererSettingsToCatalogue(renderer);
        SetTransportRouterToCatalogue(router, catalogue);
    }

    void Serializator::SetStopsToCatalogue(TransportCatalogue& catalogue) {
        auto& stops = proto_catalogue_.mutable_catalogue()->stops();
        for (const auto& stop : stops) {
            catalogue.AddStop(stop.name(), {stop.coordinates().lat(), stop.coordinates().lng()});
            stop_id_to_name_.insert({stop.id(), stop.name()});
        }
    }

    void Serializator::SetBusesToCatalogue(TransportCatalogue& catalogue) {
        auto& buses = proto_catalogue_.mutable_catalogue()->buses();
        for (const auto& bus : buses) {
            catalogue.AddBus(bus.name(), MakeBusStopsFromProto(bus), bus.type());
            bus_id_to_name_.insert({bus.id(), bus.name()});
        }
    }

    void Serializator::SetDistancesToCatalogue(TransportCatalogue& catalogue) {
        auto& distances = proto_catalogue_.mutable_catalogue()->distances();
        for(const auto distance : distances) {
            catalogue.SetDistance({stop_id_to_name_.at(distance.stop_id_from()), stop_id_to_name_.at(distance.stop_id_to())},
                distance.distance());
        }
    }

    std::vector<std::string_view> Serializator::MakeBusStopsFromProto(const transport_catalogue_serialize::Bus& bus) {
        std::vector<std::string_view> result;
        for (int i = 0; i < bus.stop_ids_size(); ++i) {
            result.push_back(stop_id_to_name_.at(bus.stop_ids(i)));
        }
        return result;
    }

    void Serializator::SetRendererSettingsToCatalogue(MapRenderer& renderer) {
        
        if (!proto_catalogue_.has_render_settings()) {
            return;
        }

        auto& proto_settings = proto_catalogue_.render_settings();

        renderer::RenderSettings settings;

        settings.size.x = proto_settings.size().x();
        settings.size.y = proto_settings.size().y();

        settings.padding = proto_settings.padding();

        settings.line_width = proto_settings.line_width();
        settings.stop_radius = proto_settings.stop_radius();

        settings.bus_label_font_size = proto_settings.bus_label_font_size();

        settings.bus_label_offset.x = proto_settings.bus_label_offset().x();
        settings.bus_label_offset.y = proto_settings.bus_label_offset().y();

        settings.stop_label_font_size = proto_settings.stop_label_font_size();

        settings.stop_label_offset.x = proto_settings.stop_label_offset().x();
        settings.stop_label_offset.y = proto_settings.stop_label_offset().y();

        settings.underlayer_color = MakeColorFromProto(proto_settings.underlayer_color());

        settings.underlayer_width = proto_settings.underlayer_width();

        auto color_pallete_size = proto_settings.color_palette_size();
        settings.color_palette.reserve(color_pallete_size);
        for (int i = 0; i < color_pallete_size; ++i) {
            settings.color_palette.push_back(MakeColorFromProto(proto_settings.color_palette(i)));
        }

        renderer.SetRenderSettings(settings);
    }

    svg::Color Serializator::MakeColorFromProto(const svg_serialize::Color& proto_color) {
        svg::Color color;
        switch (proto_color.color_case()) {
            case svg_serialize::Color::kStringColor :
                color = proto_color.string_color();
                break;
            case svg_serialize::Color::kRgbColor :
                {
                auto &proto_rgb = proto_color.rgb_color();
                color = svg::Rgb(proto_rgb.r(), proto_rgb.g(), proto_rgb.b());
                }
                break;
            case svg_serialize::Color::kRgbaColor :
                {
                auto &proto_rgba = proto_color.rgba_color();
                color = svg::Rgba(proto_rgba.r(), proto_rgba.g(), proto_rgba.b(), proto_rgba.o());
                }
                break;
            default:
                color = svg::NoneColor;
        }
        return color;
    }

    void Serializator::SetTransportRouterToCatalogue(TransportRouter& transport_router, const TransportCatalogue& catalogue) {

        if (!proto_catalogue_.has_router()) {
            return;
        }

        transport_router::RouterSettings routing_settings;
        SetRouterSettingsToCatalogue(routing_settings);

        transport_router.AddRouterSetting(routing_settings);

        transport_router.AddStopNamesAndID(stop_id_to_name_);

        auto& p_router = proto_catalogue_.router();

        SetGraphFromProto(transport_router.GetGraph());

        transport_router.GetRouter() = std::make_unique<graph::Router<transport_router::RouteWeight>>(transport_router.GetGraph(), false);
                
        SetRouterFromProto(transport_router.GetRouter());
    }

    void Serializator::SetRouterSettingsToCatalogue(transport_router::RouterSettings& routing_settings) const {
        auto &p_settings = proto_catalogue_.router().settings();

        routing_settings.bus_wait_time_ = p_settings.bus_wait_time();
        routing_settings.bus_velocity_ = p_settings.bus_velocity();
    }

    void Serializator::SetGraphFromProto(graph::DirectedWeightedGraph<transport_router::RouteWeight>& graph) {
        auto &p_graph = proto_catalogue_.router().graph();
        auto edge_count = p_graph.edges_size();

        for (auto i = 0; i < edge_count; ++i) {
            graph::Edge<transport_router::RouteWeight> edge;
            auto &p_edge = p_graph.edges(i);

            edge.from = p_edge.vertex_id_from();
            edge.to = p_edge.vertex_id_to();
            edge.weight = MakeWeightFromProto(p_edge.weight());

            graph.GetEdges().push_back(std::move(edge));
        }

        auto incidence_lists_count = p_graph.incidence_list_size();
        for (auto i = 0; i < incidence_lists_count; ++i) {
            std::vector<graph::EdgeId> list;
            auto &p_list = p_graph.incidence_list(i);
            auto list_count = p_list.edges_id_size();
            for (auto j = 0; j < list_count; ++j) {
                list.push_back(p_list.edges_id(j));
            }
            graph.GetIncidenceLists().push_back(list);
        }
    }

    void Serializator::SetRouterFromProto(std::unique_ptr<graph::Router<transport_router::RouteWeight>>& router) {

        auto &p_router = proto_catalogue_.router().router();
        auto &routes_internal_data = router->GetRoutesInternalData();

        auto routes_internal_data_count = p_router.routes_data_size();

        for (int i = 0; i < routes_internal_data_count; ++i) {
            auto &p_internal_data = p_router.routes_data(i);
            auto internal_data_count = p_internal_data.routes_internal_data_size();
            for (int j = 0; j < internal_data_count; ++j) {
                auto &p_optional_data = p_internal_data.routes_internal_data(j);
                if (p_optional_data.has_data()) {
                    graph::Router<transport_router::RouteWeight>::RouteInternalData data;
                    auto &p_data = p_optional_data.data();
                    data.weight.total_time = p_data.route_weight().total_time();
                    if (p_data.has_prev_edge()) {
                        data.prev_edge = p_data.prev_edge().edge_id();
                    } else {
                        data.prev_edge = std::nullopt;
                    }
                    routes_internal_data[i][j] = std::move(data);
                } else {
                    routes_internal_data[i][j] = std::nullopt;
                }
            }
        }
    }

    transport_router::RouteWeight Serializator::MakeWeightFromProto(const graph_serialize::RouteWeight &p_weight) const {
        transport_router::RouteWeight weight;

        weight.bus_name = bus_id_to_name_.at(p_weight.bus_name());
        weight.span_count = p_weight.span_count();
        weight.total_time = p_weight.total_time();

        return weight;
    }
}