#include "json_reader.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "request_handler.h"

#include <iostream>
#include <sstream>

int main() {
    transport_catalogue::TransportCatalogue catalogue;
    transport_catalogue::renderer::MapRenderer renderer;
    transport_catalogue::request_handler::RequestHandler request_handler(catalogue, renderer);

    std::string json;
    std::string line;

    while (std::getline(std::cin, line)) {
        json += line;
    }

    std::istringstream input(json);

    transport_catalogue::json_reader::RequestLoad(input, std::cout, request_handler, renderer, catalogue);

    input.clear();
    input.seekg(0);
    return 0 ;
}