#include "json_reader.h"
#include "request_handler.h"

#include <map>
#include <sstream>
#include <string_view>
#include <vector>

namespace transport_catalogue {

namespace json_reader {

using namespace std::literals;

json::Dict GetBusStatDict(const json::Dict& request, request_handler::RequestHandler& request_handler) {
    json::Dict response;
    const std::string bus_name = request.at("name").AsString();
    if (const auto stat = request_handler.GetBusStat(bus_name); stat) {
        response["curvature"] = (*stat).curvature;;
        response["request_id"] = request.at("id").AsInt();
        response["route_length"] = (*stat).distance;
        response["stop_count"] = (*stat).stops_on_route;
        response["unique_stop_count"] = (*stat).unique_stops;
    } else {
        response["error_message"] = "not found";
        response["request_id"] = request.at("id").AsInt();
    }
    return response;
}

json::Dict GetBusesByStop(const std::string& stop_name, request_handler::RequestHandler& request_handler) {
    json::Dict result;
    json::Array buses_array;
    const auto buses = request_handler.GetBusesByStop(stop_name);
    for (auto it = buses.begin(); it != buses.end(); ++it) {
        buses_array.push_back(std::string(*it));
    }
    result["buses"] = buses_array;
    return result;
}


json::Dict GetBusesByStopDict(const json::Dict& request, request_handler::RequestHandler& request_handler, TransportCatalogue& catalogue) {
    json::Dict response;
    json::Dict error_response;
    const std::string stop_name = request.at("name").AsString();
    if (nullptr != catalogue.FindStop(stop_name)) {
        response = GetBusesByStop(stop_name, request_handler);
        response["request_id"] = request.at("id").AsInt();
        return response;
    }
    error_response["error_message"] = "not found";
    error_response["request_id"] = request.at("id").AsInt();
    return error_response;
}

json::Dict GetRenderMap(request_handler::RequestHandler& request_handler) {
    json::Dict result;
    std::ostringstream output;
    request_handler.RenderMap().Render(output);
    result["map"] = output.str();
    return result;
}


json::Dict GetRenderMapDict(const json::Dict& request, request_handler::RequestHandler& request_handler) {
    json::Dict response;
    response = GetRenderMap(request_handler);
    response["request_id"] = request.at("id").AsInt();
    return response;
}

void ParsingStatRequest(std::ostream& out, const json::Node& request_body, request_handler::RequestHandler& request_handler, TransportCatalogue& catalogue) {
    const int size = static_cast<int>(request_body.AsArray().size());
    json::Array responses;
    for (int i = 0; i < size; ++i) {
        json::Dict request = request_body.AsArray().at(i).AsMap();
        json::Dict response;
        if (request.at("type").AsString() == "Bus"s) {
            response = GetBusStatDict(request, request_handler);
        } else if (request.at("type").AsString() == "Stop"s) {
            response = GetBusesByStopDict(request, request_handler, catalogue);
        } else if (request.at("type").AsString() == "Map"s) {
            response = GetRenderMapDict(request, request_handler);
        }
        responses.push_back(response);
    }
    json::Print(json::Document(responses), out);
}

std::vector<std::string_view> GetStops(const json::Dict& bus_dict) {
    std::vector<std::string_view> result;
    for (const auto& stop : bus_dict.at("stops").AsArray()) {
            result.push_back(stop.AsString());
        }
    if (!bus_dict.at("is_roundtrip").AsBool()) {
        for (int i = (static_cast<int>(result.size()) - 2); i >= 0; --i) {
            result.push_back(result[i]);
        }
    }
    return result;
}

domain::Bus FillBus(const json::Dict& bus_dict, TransportCatalogue& catalogue) {
    domain::Bus bus;
    bus.name = bus_dict.at("name").AsString();
    bus.is_roundtrip = bus_dict.at("is_roundtrip").AsBool();
    for (const std::string_view stop : GetStops(bus_dict)) {
        bus.stops.push_back(catalogue.FindStop(stop));
    }
    return bus;
}

void GetDistance(TransportCatalogue& catalogue, const json::Dict& stop_dict) {
    if (5u == stop_dict.size()) {
        std::string stop_name = stop_dict.at("name").AsString();
        for (const auto& [stop, distance] : stop_dict.at("road_distances").AsMap()) {
            catalogue.SetDistanceBetweenStops(stop_name, distance.AsDouble(), stop);
        }
    }
}

domain::Stop FillStop(const json::Dict& stop_dict) {
    domain::Stop stop;
    stop.name = stop_dict.at("name").AsString();
    stop.coordinates.lat = stop_dict.at("latitude").AsDouble();
    stop.coordinates.lng = stop_dict.at("longitude").AsDouble();
    return stop;
}

void ParsingBaseRequests(const json::Node& request_body, TransportCatalogue& catalogue) {
    Requests requests;
    for (const auto& base_request : request_body.AsArray()) {
        json::Dict request = base_request.AsMap();
        if (request.at("type").AsString() == "Bus"s) {
            requests.buses.push_back(std::move(request));
        } else if (request.at("type").AsString() == "Stop"s) {
            catalogue.AddStop(FillStop(request));
            requests.stops.push_back(std::move(request));
        }
    }
    for (const auto& bus_request : requests.buses) {
        catalogue.AddBus(FillBus(bus_request.AsMap(), catalogue));
    }
    requests.buses.clear();
    for (const auto& stop_request : requests.stops) {
        GetDistance(catalogue, stop_request.AsMap());
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
    json::Dict setting_dict = request_body.AsMap();
    std::vector<svg::Color> color_palette;
    for (const auto& color_node: setting_dict.at("color_palette").AsArray()) {
        color_palette.push_back(SetColor(color_node));
    }
    renderer::RenderSettings render_settings{
        setting_dict.at("width").AsDouble(),
        setting_dict.at("height").AsDouble(),
        setting_dict.at("padding").AsDouble(),
        setting_dict.at("line_width").AsDouble(),
        setting_dict.at("stop_radius").AsDouble(),
        setting_dict.at("bus_label_font_size").AsInt(),
        {setting_dict.at("bus_label_offset").AsArray()[0].AsDouble(), setting_dict.at("bus_label_offset").AsArray()[1].AsDouble()},
        setting_dict.at("stop_label_font_size").AsInt(),
        {setting_dict.at("stop_label_offset").AsArray()[0].AsDouble(), setting_dict.at("stop_label_offset").AsArray()[1].AsDouble()},
        SetColor(setting_dict.at("underlayer_color")),
        setting_dict.at("underlayer_width").AsDouble(),
        color_palette
    };
    renderer.AddSettings(render_settings);
}

void RequestLoad(std::istream& in, std::ostream& out,
                    request_handler::RequestHandler& request_handler,
                    renderer::MapRenderer& renderer,
                    TransportCatalogue& catalogue) {
    json::Document requests = json::Load(in);
    for (const auto& [request_type, request_body] : requests.GetRoot().AsMap()) {
        if (request_type == "base_requests"s) {
            ParsingBaseRequests(request_body, catalogue);
        } else if (request_type == "stat_requests"s) {
            ParsingStatRequest(out, request_body, request_handler, catalogue);
        } else if (request_type == "render_settings"s){
            ParsingRenderSettings(request_body, renderer);
        }
    }
}

} // namespace json_reader
} // namespace transport_catalogue