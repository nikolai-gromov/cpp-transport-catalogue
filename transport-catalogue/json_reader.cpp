#include "json_reader.h"
#include "request_handler.h"

#include <map>
#include <sstream>
#include <string_view>
#include <vector>

namespace transport_catalogue {

namespace json_reader {

using namespace std::literals;

void GetBusStatDict(const json::Dict& request,
                    json::Builder& builder,
                    const request_handler::RequestHandler& request_handler) {
    if (const auto stat = request_handler.GetBusStat(request.at("name"s).AsString()); stat) {
        builder.StartDict()
            .Key("curvature"s).Value((*stat).curvature)
            .Key("request_id"s).Value(request.at("id"s).AsInt())
            .Key("route_length"s).Value((*stat).distance)
            .Key("stop_count"s).Value((*stat).stops_on_route)
            .Key("unique_stop_count"s).Value((*stat).unique_stops)
        .EndDict();
    } else {
        builder.StartDict()
            .Key("error_message"s).Value("not found"s)
            .Key("request_id"s).Value(request.at("id"s).AsInt())
        .EndDict();
    }
}

void GetBusesByStopDict(const json::Dict& request,
                        json::Builder& builder,
                        const TransportCatalogue& catalogue,
                        const request_handler::RequestHandler& request_handler) {
    const std::string stop_name = request.at("name"s).AsString();
    if (nullptr != catalogue.FindStop(stop_name)) {
        builder.StartDict()
            .Key("buses"s).StartArray();
                const auto buses = request_handler.GetBusesByStop(stop_name);
                for (auto it = buses.begin(); it != buses.end(); ++it) {
                    builder.Value(std::string(*it));
                }
            builder.EndArray()
            .Key("request_id"s).Value(request.at("id"s).AsInt())
        .EndDict();
    } else {
        builder.StartDict()
            .Key("error_message"s).Value("not found"s)
            .Key("request_id"s).Value(request.at("id"s).AsInt())
        .EndDict();
    }
}

void GetRenderMapDict(const json::Dict& request,
                        json::Builder& builder,
                        const request_handler::RequestHandler& request_handler) {
    std::ostringstream output;
    request_handler.RenderMap().Render(output);
    builder.StartDict()
        .Key("map"s).Value(output.str())
        .Key("request_id"s).Value(request.at("id"s).AsInt())
    .EndDict();
}

void GetRouteItemDict(json::Builder& builder, const router::Item& item) {
    if (item.type == router::ItemType::WAIT) {
        builder.StartDict()
            .Key("type").Value("Wait"s)
            .Key("stop_name"s).Value(std::string(item.name))
            .Key("time"s).Value(item.time)
        .EndDict();
    } else if (item.type == router::ItemType::BUS) {
        builder.StartDict()
            .Key("type"s).Value("Bus"s)
            .Key("bus"s).Value(std::string(item.name))
            .Key("span_count"s).Value(item.span_count)
            .Key("time"s).Value(item.time)
        .EndDict();
    }
}

void GetRouteItemsDict(json::Builder& builder, const std::optional<router::RouteItems>& items, int request_id) {
    if (items) {
        builder.StartDict()
            .Key("request_id").Value(request_id)
            .Key("total_time").Value(items.value().total_time)
            .Key("items").StartArray();
                for (const auto& item : items.value().items) {
                    GetRouteItemDict(builder, item);
                }
            builder.EndArray();
        builder.EndDict();
    } else {
        builder.StartDict()
            .Key("request_id"s).Value(request_id)
            .Key("error_message"s).Value("not found"s)
        .EndDict();
    }
}

void ParsingStatRequest(std::ostream& out,
                        const json::Node& request_body,
                        const TransportCatalogue& catalogue,
                        const request_handler::RequestHandler& handler) {
    json::Builder builder;
    builder.StartArray();
    const int size = static_cast<int>(request_body.AsArray().size());
    for (int i = 0; i < size; ++i) {
        json::Dict request = request_body.AsArray().at(i).AsDict();
        if (request.at("type"s).AsString() == "Bus"s) {
            GetBusStatDict(request, builder, handler);
        } else if (request.at("type"s).AsString() == "Stop"s) {
            GetBusesByStopDict(request, builder, catalogue, handler);
        } else if (request.at("type"s).AsString() == "Map"s) {
            GetRenderMapDict(request, builder, handler);
        } else if (request.at("type"s).AsString() == "Route"s) {
            GetRouteItemsDict(builder, handler.GetRouteByStops(request["from"s].AsString(), request["to"s].AsString()), request["id"s].AsInt());
        }
    }
    builder.EndArray();
    json::Print(json::Document(builder.Build()), out);
}

std::vector<std::string_view> GetStops(const json::Dict& bus_dict) {
    std::vector<std::string_view> result;
    for (const auto& stop : bus_dict.at("stops"s).AsArray()) {
            result.push_back(stop.AsString());
        }
    if (!bus_dict.at("is_roundtrip"s).AsBool()) {
        for (int i = (static_cast<int>(result.size()) - 2); i >= 0; --i) {
            result.push_back(result[i]);
        }
    }
    return result;
}

domain::Bus FillBus(const json::Dict& bus_dict, const TransportCatalogue& catalogue) {
    domain::Bus bus;
    bus.name = bus_dict.at("name"s).AsString();
    bus.is_roundtrip = bus_dict.at("is_roundtrip"s).AsBool();
    for (const std::string_view stop : GetStops(bus_dict)) {
        bus.stops.push_back(catalogue.FindStop(stop));
    }
    return bus;
}

void GetDistance(const json::Dict& stop_dict, TransportCatalogue& catalogue) {
    if (5u == stop_dict.size()) {
        std::string stop_name = stop_dict.at("name"s).AsString();
        for (const auto& [stop, distance] : stop_dict.at("road_distances"s).AsDict()) {
            catalogue.SetDistanceBetweenStops(stop_name, stop, distance.AsDouble());
        }
    }
}

domain::Stop FillStop(const json::Dict& stop_dict) {
    domain::Stop stop;
    stop.name = stop_dict.at("name"s).AsString();
    stop.coordinates.lat = stop_dict.at("latitude"s).AsDouble();
    stop.coordinates.lng = stop_dict.at("longitude"s).AsDouble();
    return stop;
}

void ParsingBaseRequests(const json::Node& request_body, TransportCatalogue& catalogue) {
    Requests requests;
    for (const auto& base_request : request_body.AsArray()) {
        json::Dict request = base_request.AsDict();
        if (request.at("type"s).AsString() == "Bus"s) {
            requests.buses.push_back(std::move(request));
        } else if (request.at("type"s).AsString() == "Stop"s) {
            catalogue.AddStop(FillStop(request));
            requests.stops.push_back(std::move(request));
        }
    }
    for (const auto& bus_request : requests.buses) {
        catalogue.AddBus(FillBus(bus_request.AsDict(), catalogue));
    }
    requests.buses.clear();
    for (const auto& stop_request : requests.stops) {
        GetDistance(stop_request.AsDict(), catalogue);
    }
    requests.stops.clear();
}

svg::Color SetColor(const json::Node& color_node) {
    svg::Color color;
    if (color_node.IsString()) {
        color = color_node.AsString();
    } else if (color_node.AsArray().size() == 3) {
        color = svg::Rgb(
                        color_node.AsArray()[0].AsDouble(),
                        color_node.AsArray()[1].AsDouble(),
                        color_node.AsArray()[2].AsDouble());
    } else if (color_node.AsArray().size() == 4) {
        color = svg::Rgba(
                        color_node.AsArray()[0].AsDouble(),
                        color_node.AsArray()[1].AsDouble(),
                        color_node.AsArray()[2].AsDouble(),
                        color_node.AsArray()[3].AsDouble());
    }
    return color;
}

void ParsingRenderSettings(const json::Node& request_body, renderer::MapRenderer& renderer) {
    json::Dict setting_dict = request_body.AsDict();
    std::vector<svg::Color> color_palette;
    for (const auto& color_node: setting_dict.at("color_palette"s).AsArray()) {
        color_palette.push_back(SetColor(color_node));
    }
    renderer::RenderSettings render_settings{
        setting_dict.at("width"s).AsDouble(),
        setting_dict.at("height"s).AsDouble(),
        setting_dict.at("padding"s).AsDouble(),
        setting_dict.at("line_width"s).AsDouble(),
        setting_dict.at("stop_radius"s).AsDouble(),
        setting_dict.at("bus_label_font_size"s).AsInt(),
        {setting_dict.at("bus_label_offset"s).AsArray()[0].AsDouble(), setting_dict.at("bus_label_offset"s).AsArray()[1].AsDouble()},
        setting_dict.at("stop_label_font_size"s).AsInt(),
        {setting_dict.at("stop_label_offset"s).AsArray()[0].AsDouble(), setting_dict.at("stop_label_offset"s).AsArray()[1].AsDouble()},
        SetColor(setting_dict.at("underlayer_color"s)),
        setting_dict.at("underlayer_width"s).AsDouble(),
        color_palette
    };
    renderer.AddSettings(render_settings);
}

void ParsingRoutingSettings(const json::Node& request_body, router::TransportRouter& router) {
    json::Dict setting_dict = request_body.AsDict();
    router.SetRoutingSettings({ setting_dict.at("bus_wait_time").AsDouble(), setting_dict.at("bus_velocity").AsDouble() });
}

void RequestLoad(std::istream& in, std::ostream& out,
                    TransportCatalogue& catalogue,
                    renderer::MapRenderer& renderer,
                    router::TransportRouter& router,
                    request_handler::RequestHandler& handler) {
    json::Document requests = json::Load(in);
    for (const auto& [request_type, request_body] : requests.GetRoot().AsDict()) {
        if (request_type == "base_requests"s) {
            ParsingBaseRequests(request_body, catalogue);
        } else if (request_type == "stat_requests"s) {
            router.BuildRoutes();
            ParsingStatRequest(out, request_body, catalogue, handler);
        } else if (request_type == "render_settings"s){
            ParsingRenderSettings(request_body, renderer);
        } else if (request_type == "routing_settings"s){
            ParsingRoutingSettings(request_body, router);
        }
    }
}

}  // namespace json_reader
}  // namespace transport_catalogue