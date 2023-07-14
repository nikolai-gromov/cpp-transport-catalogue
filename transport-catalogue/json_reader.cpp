#include "json_reader.h"
#include "request_handler.h"

#include <map>
#include <sstream>
#include <string_view>
#include <vector>

namespace transport_catalogue {

namespace json_reader {

using namespace std::literals;


void PrintBusStat(std::ostream& out, const json::Dict& request, request_handler::RequestHandler& request_handler, bool comma) {
    if (const auto stat = request_handler.GetBusStat(request.at("name").AsString()); stat) {
        out << "    {\n";
        out << "        \"curvature\": " << (*stat).curvature << ",\n";
        out << "        \"request_id\": " << request.at("id").AsInt() << ",\n";
        out << "        \"route_length\": " << (*stat).distance << ",\n";
        out << "        \"stop_count\": " << (*stat).stops_on_route << ",\n";
        out << "        \"unique_stop_count\": " << (*stat).unique_stops << "\n";
    } else {
        out << "    {\n";
        out << "        \"error_message\": " << "\"not found\"," << "\n";
        out << "        \"request_id\": " << request.at("id").AsInt() << "\n";
    }
    if (!comma) {
        out << "    }" << std::endl;
    } else {
        out << "    }," << std::endl;
    }
}

void PrintBusesByStop(std::ostream& out, const json::Dict& request, request_handler::RequestHandler& request_handler, TransportCatalogue& catalogue, bool comma) {
    if (nullptr != catalogue.FindStop(request.at("name").AsString())) {
        const auto buses = request_handler.GetBusesByStop(request.at("name").AsString());
        out << "    {\n";
        out << "        \"buses\": [\n";
        out << "            ";
            for (auto it = buses.begin(); it != buses.end(); ++it) {
                if (std::next(it) != buses.end()) {
                    out << "\"" << (*it) << "\", ";
                } else {
                    out << "\"" << (*it) << "\"";
                }
            }
        out << "\n";
        out << "        ],\n";
        out << "        \"request_id\": " << request.at("id").AsInt() << "\n";
    } else {
        out << "    {\n";
        out << "        \"error_message\": " << "\"not found\"," << "\n";
        out << "        \"request_id\": " << request.at("id").AsInt() << "\n";
    }
    if (!comma) {
        out << "    }" << std::endl;
    } else {
        out << "    }," << std::endl;
    }
}

void PrintRenderMap(std::ostream& out, const json::Dict& request, request_handler::RequestHandler& request_handler, bool comma) {
    std::ostringstream output;
    request_handler.RenderMap().Render(output);
    out << "    {\n";
    out << "        \"map\": ";
    json::Print(json::Document(output.str()), out);
    out << ",\n";
    out << "        \"request_id\": " << request.at("id").AsInt() << "\n";
    if (!comma) {
        out << "    }" << std::endl;
    } else {
        out << "    }," << std::endl;
    }
}

void ParsingStatRequest(std::ostream& out, const json::Node& request_body, request_handler::RequestHandler& request_handler, TransportCatalogue& catalogue) {
    int size = static_cast<int>(request_body.AsArray().size());
    out << "["<< std::endl;
    for (int i = 0; i < size; ++i) {
        json::Dict request = request_body.AsArray().at(i).AsMap();
        if (i < (size - 1)) {
            if (request.at("type").AsString() == "Bus"s) {
                PrintBusStat(out, request, request_handler, true);
            } else if (request.at("type").AsString() == "Stop"s) {
                PrintBusesByStop(out, request, request_handler, catalogue, true);
            } else if (request.at("type").AsString() == "Map"s) {
                PrintRenderMap(out, request, request_handler, true);
            }
        } else if (i == (size - 1)) {
            if (request.at("type").AsString() == "Bus"s) {
                PrintBusStat(out, request, request_handler);
            } else if (request.at("type").AsString() == "Stop"s) {
                PrintBusesByStop(out, request, request_handler, catalogue);
            } else if (request.at("type").AsString() == "Map"s) {
                PrintRenderMap(out, request, request_handler);
            }
        }
    }
    out << "]" << std::endl;
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