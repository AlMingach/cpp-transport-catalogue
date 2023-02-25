#include "map_renderer.h"

bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

namespace renderer {

    void MapRenderer::SetRenderSettings(RenderSettings settings) {
        settings_ = settings;
    }

    std::optional<RenderSettings> MapRenderer::GetValue() const {
        return settings_;
    }


    void MapRenderer::RenderBusLine(const Buses& buses_data, svg::Document& result, const SphereProjector& proj) const {
        size_t color_index = 0;
        size_t max_color_index = settings_.value().color_palette.size();
        for (const auto& bus : buses_data) {
            if (bus.second->stops_.size() > 0) {
                svg::Polyline line;
                std::vector<geo::Coordinates> geo_coords;
                const auto& settings = settings_.value();
                line.SetStrokeColor(settings.color_palette.at(color_index % max_color_index)).
                    SetFillColor(svg::NoneColor).SetStrokeWidth(settings.line_width).
                    SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                geo_coords.reserve(bus.second->stops_.size());
                for (const auto stop : bus.second->stops_) {
                    geo_coords.push_back(stop->coordinates_);
                }
                if (!bus.second->round_) {
                    for (auto iter = std::next(bus.second->stops_.rbegin()); iter < bus.second->stops_.rend(); ++iter) {
                        geo_coords.push_back((*iter)->coordinates_);
                    }
                }
                for (const auto geo_coord : geo_coords) {
                    line.AddPoint(proj(geo_coord));
                }
                result.Add(line);
                ++color_index;
            }
        }
    }

    void MapRenderer::RenderBusName(const Buses& buses_data, svg::Document& result, const SphereProjector& proj) const {
        using namespace std::literals;
        svg::Text text_substrate;
        svg::Text text;
        size_t color_index = 0;
        size_t max_color_index = settings_.value().color_palette.size();
        const auto& settings = settings_.value();

        text.SetOffset(settings.bus_label_offset).
            SetFontSize(settings.bus_label_font_size).SetFontFamily("Verdana"s).SetFontWeight("bold"s);

        text_substrate = text;

        text_substrate.SetFillColor(settings.underlayer_color).SetStrokeColor(settings.underlayer_color).
            SetStrokeWidth(settings.underlayer_width).SetStrokeLineCap(svg::StrokeLineCap::ROUND).
            SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        for (const auto& bus : buses_data) {
            if (bus.second->stops_.size() > 0) {

                const auto& stop_coordinate = bus.second->stops_.at(0)->coordinates_;

                result.Add(text_substrate.SetPosition(proj(stop_coordinate)).SetData(std::string(bus.first)));

                result.Add(text.SetPosition(proj(stop_coordinate)).SetData(std::string(bus.first))
                    .SetFillColor(settings.color_palette.at(color_index % max_color_index)));

                if (!bus.second->round_) {
                    const auto& stop_end = (*bus.second->stops_.rbegin())->coordinates_;
                    if (stop_end != stop_coordinate) {
                        result.Add(text_substrate.SetPosition(proj(stop_end)).SetData(std::string(bus.first)));
                        result.Add(text.SetPosition(proj(stop_end)).SetData(std::string(bus.first))
                            .SetFillColor(settings.color_palette.at(color_index % max_color_index)));
                    }
                }
                ++color_index;
            }
        }
    }

    void MapRenderer::RenderStopCoordinates(const Stops& stops_data, svg::Document& result, const SphereProjector& proj) const {
        using namespace std::literals;
        svg::Circle stop_circle;
        stop_circle.SetRadius(settings_.value().stop_radius).SetFillColor("white"s);
        for (const auto& stop : stops_data) {
            result.Add(stop_circle.SetCenter(proj(stop.second->coordinates_)));
        }
    }

    void MapRenderer::RenderStopNames(const Stops& stops_data, svg::Document& result, const SphereProjector& proj) const {
        using namespace std::literals;
        svg::Text text_substrate;
        svg::Text text;
        const auto& settings = settings_.value();

        text.SetOffset(settings.stop_label_offset).
            SetFontSize(settings.stop_label_font_size).SetFontFamily("Verdana"s);

        text_substrate = text;

        text_substrate.SetFillColor(settings.underlayer_color).SetStrokeColor(settings.underlayer_color).
            SetStrokeWidth(settings.underlayer_width).SetStrokeLineCap(svg::StrokeLineCap::ROUND).
            SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        text.SetFillColor("black"s);

        for (const auto& stop : stops_data) {
            const auto& coordinates = stop.second->coordinates_;
            result.Add(text_substrate.SetPosition(proj(coordinates)).SetData(std::string(stop.first)));
            result.Add(text.SetPosition(proj(coordinates)).SetData(std::string(stop.first)));
        }
    }

    svg::Document MapRenderer::RenderMap(const Buses& buses_data, const Stops& stops_data)const {
        svg::Document result;
        std::vector<geo::Coordinates> all_geo_coords;
        for (const auto& stop : stops_data) {
            all_geo_coords.push_back(stop.second->coordinates_);
        }
        const SphereProjector proj(
            all_geo_coords.begin(), all_geo_coords.end(), settings_.value().size.x, settings_.value().size.y, settings_.value().padding
        );

        RenderBusLine(buses_data, result, proj);
        RenderBusName(buses_data, result, proj);
        RenderStopCoordinates(stops_data, result, proj);
        RenderStopNames(stops_data, result, proj);

        return result;
    }
}