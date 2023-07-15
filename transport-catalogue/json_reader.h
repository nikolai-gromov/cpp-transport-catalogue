#pragma once
#include "json.h"
#include "domain.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "request_handler.h"

#include <iostream>

namespace transport_catalogue {

namespace json_reader{

struct Requests {
    std::vector<json::Node> buses;
    std::vector<json::Node> stops;
};

json::Dict GetBusStatDict(const json::Dict& request, request_handler::RequestHandler& request_handler);

json::Dict GetBusesByStop(const std::string& stop_name, request_handler::RequestHandler& request_handler);

json::Dict GetBusesByStopDict(const json::Dict& request, request_handler::RequestHandler& request_handler, TransportCatalogue& catalogue);

json::Dict GetRenderMap(request_handler::RequestHandler& request_handler);

json::Dict GetRenderMapDict(const json::Dict& request, request_handler::RequestHandler& request_handler);

void ParsingStatRequest(std::ostream& out, const json::Node& request_body, request_handler::RequestHandler& request_handler, TransportCatalogue& catalogue);

std::vector<std::string_view> GetStops(const json::Dict& bus_dict);

domain::Bus FillBus(const json::Dict& bus_dict, TransportCatalogue& catalogue);

void GetDistance(TransportCatalogue& catalogue, const json::Dict& stop_dict);

domain::Stop FillStop(const json::Dict& stop_dict);

void ParsingBaseRequests(const json::Node& request_body, TransportCatalogue& catalogue);

svg::Color SetColor(const json::Node& color_node);

void ParsingRenderSettings(const json::Node& request_body, renderer::MapRenderer& renderer);

void RequestLoad(std::istream& in, std::ostream& out,
                    request_handler::RequestHandler& request_handler,
                    renderer::MapRenderer& renderer,
                    TransportCatalogue& catalogue);

} // namespace json_reader
} // namespace transport_catalogue