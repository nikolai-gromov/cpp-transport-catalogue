#include "json_reader.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"
#include "request_handler.h"

#include <iostream>
#include <sstream>

int main() {
    transport_catalogue::TransportCatalogue catalogue;
    transport_catalogue::renderer::MapRenderer renderer;
    transport_catalogue::router::TransportRouter router(catalogue);
    transport_catalogue::request_handler::RequestHandler handler(catalogue, renderer, router);

    std::string json;
    std::string line;

    while (std::getline(std::cin, line)) {
        json += line;
    }

    std::istringstream input(json);

    transport_catalogue::json_reader::RequestLoad(input, std::cout, catalogue, renderer, router, handler);

    input.clear();
    input.seekg(0);
    return 0;
}