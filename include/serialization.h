#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

#include "map_renderer.h"
#include "transport_catalogue.h"
#include "transport_router.h"
#include "transport_catalogue.pb.h"


namespace serialize {

    struct SerializeSettings {
        std::filesystem::path path;
    };

    class Serializator {
    public:
        using ProtoTransportCatalogue = transport_catalogue_serialize::TransportCatalogue;
        using TransportCatalogue = transport_catalogue::TransportCatalogue;
        using ProtoCoordinates = transport_catalogue_serialize::Coordinates;
        using MapRenderer = renderer::MapRenderer;
        using TransportRouter = transport_router::TransportRouter;

        void SetSerializeSettings(SerializeSettings settings);

        void AddTransportCatalogue(const TransportCatalogue& catalogue);

        void AddRenderSettings(const MapRenderer& renderer);

        void SetProtoTransportRouter(const TransportRouter& router);

        void Serialize() const;

        void Deserialize(TransportCatalogue& catalogue, MapRenderer& renderer, TransportRouter& router);

    private:

        SerializeSettings settings_;

        ProtoTransportCatalogue proto_catalogue_;

        std::unordered_map<std::string_view, int> stop_name_to_id_;
        std::unordered_map<int, std::string_view> stop_id_to_name_;
        std::unordered_map<std::string_view, int> bus_name_to_id_;
        std::unordered_map<int, std::string_view> bus_id_to_name_;

        void SetProtoStops(const TransportCatalogue& catalogue, ProtoTransportCatalogue& proto_catalogue);
        ProtoCoordinates MakeCoordinatesToProto(const geo::Coordinates &coordinates);
        void SetProtoBuses(const TransportCatalogue& catalogue, ProtoTransportCatalogue& proto_catalogue);
        void SetProtoDistances(const TransportCatalogue& catalogue, ProtoTransportCatalogue& proto_catalogue);

        void SetStopsToCatalogue(TransportCatalogue& catalogue);
        void SetBusesToCatalogue(TransportCatalogue& catalogue);
        void SetDistancesToCatalogue(TransportCatalogue& catalogue);

        svg_serialize::Color MakeColorToProto(const svg::Color &color);
        svg::Color MakeColorFromProto(const svg_serialize::Color &p_color);
        void SetRendererSettingsToCatalogue(MapRenderer& renderer);

        std::vector<std::string_view> MakeBusStopsFromProto(const transport_catalogue_serialize::Bus& bus);

        void AddTransportRouterSettings(const transport_router::RouterSettings& settings);
        void AddGraph(const graph::DirectedWeightedGraph<transport_router::RouteWeight>& graph);
        void AddRouter(const std::unique_ptr<graph::Router<transport_router::RouteWeight>>& router);

        void SetTransportRouterToCatalogue(TransportRouter &transport_router, const TransportCatalogue& catalogue);
        void SetRouterSettingsToCatalogue(transport_router::RouterSettings& routing_settings) const;
        void SetGraphFromProto(graph::DirectedWeightedGraph<transport_router::RouteWeight>& graph);
        transport_router::RouteWeight MakeWeightFromProto(const graph_serialize::RouteWeight &p_weight) const;

        void SetRouterFromProto(std::unique_ptr<graph::Router<transport_router::RouteWeight>>& router);
    };

} //namespace serialize