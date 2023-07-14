#include "map_renderer.h"

#include <map>

namespace transport_catalogue {

namespace renderer {

using namespace std::literals;

bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

void MapRenderer::AddSettings(const RenderSettings& render_settings) {
    render_settings_ = render_settings;
}

svg::Document MapRenderer::RenderMap(const std::unordered_map<std::string_view, BusPtr>& buses_dict) const {
    svg::Document svg_doc;
    std::map<std::string_view, BusPtr> buses(buses_dict.begin(), buses_dict.end());
    auto comp = [] (StopPtr lhs, StopPtr rhs) {
                                             return lhs->name < rhs->name;
                     };
    std::set<StopPtr, decltype(comp)> stops(comp);
    std::vector<geo::Coordinates> geo_coords;
    for (const auto& [bus_name, bus_ptr] : buses) {
        for (const auto& stops_ptr : bus_ptr->stops) {
            stops.insert(stops_ptr);
            geo_coords.push_back(stops_ptr->coordinates);
        }
    }
    SphereProjector projector(geo_coords.begin(), geo_coords.end(),
                              render_settings_.width,
                              render_settings_.height,
                              render_settings_.padding);
    int route_count = 0;
    for (const auto& [bus_name, bus_ptr] : buses) {
        svg_doc.Add(RenderRoute(bus_ptr, projector, route_count));
        ++route_count;
    }
    route_count = 0;
    for (const auto& [bus_name, bus_ptr] : buses) {
        if (bus_ptr->stops.size() == 0) {
            break;
        }
        svg_doc.Add(RenderBusNameBackground(bus_ptr, projector(bus_ptr->stops[0]->coordinates)));
        svg_doc.Add(RenderBusName(bus_ptr, projector(bus_ptr->stops[0]->coordinates), route_count));
        if (!bus_ptr->is_roundtrip) {
            int pos_final_stop = static_cast<int>(bus_ptr->stops.size()) / 2;
            if (bus_ptr->stops[0] != bus_ptr->stops[pos_final_stop]) {
                svg_doc.Add(RenderBusNameBackground(bus_ptr, projector(bus_ptr->stops[pos_final_stop]->coordinates)));
                svg_doc.Add(RenderBusName(bus_ptr, projector(bus_ptr->stops[pos_final_stop]->coordinates), route_count));
            }
        }
        ++route_count;
    }
    for (StopPtr stop_ptr : stops) {
        svg_doc.Add(RenderStopSymbols(projector(stop_ptr->coordinates)));
    }
    for (StopPtr stop_ptr : stops) {
        svg_doc.Add(RenderStopNameBackground(stop_ptr,  projector(stop_ptr->coordinates)));
        svg_doc.Add(RenderStopName(stop_ptr,  projector(stop_ptr->coordinates)));
    }
    return svg_doc;
}

svg::Polyline MapRenderer::RenderRoute(BusPtr bus_ptr,  SphereProjector& projector, int color_number) const {
    svg::Polyline route;
    for (const auto& stop_ptr : bus_ptr->stops) {
        route.AddPoint(projector(stop_ptr->coordinates));
    }
    route.SetFillColor("none");
    route.SetStrokeColor(render_settings_.color_palette[color_number % render_settings_.color_palette.size()]);
    route.SetStrokeWidth(render_settings_.line_width);
    route.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    route.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    return route;
}

svg::Text MapRenderer::RenderBusNameBackground(BusPtr bus_ptr,  svg::Point position) const {
    svg::Text route_name_base;
    route_name_base.SetPosition(position);
    route_name_base.SetOffset(render_settings_.bus_label_offset);
    route_name_base.SetFontSize(render_settings_.bus_label_font_size);
    route_name_base.SetFontFamily("Verdana"s);
    route_name_base.SetFontWeight("bold"s);
    route_name_base.SetData(bus_ptr->name);
    route_name_base.SetFillColor(render_settings_.underlayer_color);
    route_name_base.SetStrokeColor(render_settings_.underlayer_color);
    route_name_base.SetStrokeWidth(render_settings_.underlayer_width);
    route_name_base.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    route_name_base.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    return route_name_base;
}

svg::Text MapRenderer::RenderBusName(BusPtr bus_ptr,  svg::Point position, int color_number) const {
    svg::Text route_name;
    route_name.SetPosition(position);
    route_name.SetOffset(render_settings_.bus_label_offset);
    route_name.SetFontSize(render_settings_.bus_label_font_size);
    route_name.SetFontFamily("Verdana"s);
    route_name.SetFontWeight("bold"s);
    route_name.SetData(bus_ptr->name);
    route_name.SetFillColor(render_settings_.color_palette[color_number % render_settings_.color_palette.size()]);
    return route_name;
}

svg::Circle MapRenderer::RenderStopSymbols(svg::Point position) const {
    svg::Circle stop_symbol;
    stop_symbol.SetCenter(position);
    stop_symbol.SetRadius(render_settings_.stop_radius);
    stop_symbol.SetFillColor("white"s);
    return stop_symbol;
}

svg::Text MapRenderer::RenderStopNameBackground(StopPtr stop_ptr, svg::Point position) const {
    svg::Text stop_name_base;
    stop_name_base.SetPosition(position);
    stop_name_base.SetOffset(render_settings_.stop_label_offset);
    stop_name_base.SetFontSize(render_settings_.stop_label_font_size);
    stop_name_base.SetFontFamily("Verdana"s);
    stop_name_base.SetData(stop_ptr->name);
    stop_name_base.SetFillColor(render_settings_.underlayer_color);
    stop_name_base.SetStrokeColor(render_settings_.underlayer_color);
    stop_name_base.SetStrokeWidth(render_settings_.underlayer_width);
    stop_name_base.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    stop_name_base.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    return stop_name_base;
}

svg::Text MapRenderer::RenderStopName(StopPtr stop_ptr, svg::Point position) const {
    svg::Text stop_name;
    stop_name.SetPosition(position);
    stop_name.SetOffset(render_settings_.stop_label_offset);
    stop_name.SetFontSize(render_settings_.stop_label_font_size);
    stop_name.SetFontFamily("Verdana"s);
    stop_name.SetData(stop_ptr->name);
    stop_name.SetFillColor("black"s);
    return stop_name;
}

} // namespace renderer
} // namespace transport_catalogue