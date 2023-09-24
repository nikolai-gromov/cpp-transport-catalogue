#include "json_reader.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"
#include "request_handler.h"
#include "serialization.h"

#include <iostream>
#include <sstream>
#include <string_view>

using namespace std::literals;

/*void TestMakeBase() {
    transport_catalogue::TransportCatalogue catalogue;
    transport_catalogue::renderer::MapRenderer renderer;
    transport_catalogue::router::TransportRouter router(catalogue);
    transport_catalogue::serialization::Serializer serializer(catalogue, renderer, router);

    std::string json = "{\"serialization_settings\": {\"file\": \"transport_catalogue.db\"}, \"routing_settings\": {\"bus_wait_time\": 217, \"bus_velocity\": 955}, \"render_settings\": {\"width\": 77422.96733606106, \"height\": 1601.1384971945422, \"padding\": 259.91647133612435, \"stop_radius\": 44265.42608354646, \"line_width\": 64943.2259679043, \"stop_label_font_size\": 6097, \"stop_label_offset\": [49340.30109391257, -21518.661058272424], \"underlayer_color\": \"brown\", \"underlayer_width\": 96963.46029105314, \"color_palette\": [\"cyan\", \"green\", [148, 92, 15, 0.4202188217339713], [13, 81, 176], [89, 111, 90, 0.7134566375060595], \"pink\", [176, 164, 80], [76, 68, 43], \"blue\", [208, 118, 38], \"gold\", [245, 111, 63], \"tan\", [159, 241, 126], \"olive\", [179, 134, 80, 0.9683534092949545], \"red\", [70, 158, 41, 0.36231310218956303], [136, 206, 31], [254, 221, 32], [223, 215, 199], [173, 151, 44], [34, 199, 94], \"lime\", \"olive\", [213, 94, 244], [231, 21, 109, 0.7321584099798798], [198, 11, 167], \"purple\", [22, 203, 158], [227, 144, 94, 0.8410831694221149], \"white\", [217, 167, 154, 0.32466970705831866], [208, 144, 108], [50, 242, 72, 0.6222557021798093], [8, 40, 174, 0.6420765478393026], [136, 140, 206], \"lavender\", \"magenta\", [144, 238, 111, 0.14431197126073525], \"brown\", [214, 58, 200], \"blue\", \"black\", \"green\", \"peru\", \"coral\", [111, 1, 82], [146, 43, 130, 0.04305892992523086], [72, 64, 199], \"gray\", [244, 108, 233], [217, 43, 53, 0.9847972025458727], \"aqua\", \"yellow\", [227, 53, 12, 0.7987754492192862], \"maroon\", \"red\", \"violet\", [188, 251, 228], [195, 201, 49], [255, 120, 164], \"aqua\", \"gray\", \"lime\", \"wheat\", [173, 127, 117, 0.527014280067947], [160, 219, 40, 0.6076302525276452], \"thistle\", \"thistle\", [143, 186, 24, 0.4676141148800901], [38, 114, 64], [197, 180, 8], [139, 201, 99], \"cyan\", [77, 201, 49, 0.4897483401620385], [91, 132, 251], \"orchid\", [181, 38, 26], [59, 212, 49], \"gray\", \"purple\", \"plum\", [244, 253, 255], [102, 160, 231, 0.08459829075198733], [18, 91, 174]], \"bus_label_font_size\": 44010, \"bus_label_offset\": [-35407.91124531504, 86163.95635806938]}, \"base_requests\": [{\"type\": \"Stop\", \"name\": \"Gz\", \"latitude\": 46.06455394608965, \"longitude\": 37.68383286894389, \"road_distances\": {\"SATi1wM6953xUoVovIP\": 450021}}, {\"type\": \"Stop\", \"name\": \"SATi1wM6953xUoVovIP\", \"latitude\": 42.559135105838934, \"longitude\": 37.9335955156967, \"road_distances\": {\"Gz\": 751991}}, {\"type\": \"Bus\", \"name\": \"yklV2Ksfbav358r\", \"stops\": [\"SATi1wM6953xUoVovIP\", \"Gz\", \"SATi1wM6953xUoVovIP\"], \"is_roundtrip\": true}, {\"type\": \"Bus\", \"name\": \"n72ax8xpV5K\", \"stops\": [\"SATi1wM6953xUoVovIP\", \"Gz\", \"SATi1wM6953xUoVovIP\"], \"is_roundtrip\": true}, {\"type\": \"Bus\", \"name\": \"cOk0uPHhw3PFSZFI4J2uxzUbq\", \"stops\": [\"SATi1wM6953xUoVovIP\", \"Gz\"], \"is_roundtrip\": false}]}";
    std::istringstream input(json);

    transport_catalogue::json_reader::MakeBaseLoad(input, catalogue, renderer, router, serializer);
}

void TestProcessRequests() {
    transport_catalogue::TransportCatalogue catalogue;
    transport_catalogue::renderer::MapRenderer renderer;
    transport_catalogue::router::TransportRouter router(catalogue);
    transport_catalogue::request_handler::RequestHandler handler(catalogue, renderer, router);
    transport_catalogue::serialization::Serializer serializer(catalogue, renderer, router);

    std::string json = "{\"serialization_settings\": {\"file\": \"transport_catalogue.db\"}, \"stat_requests\": [{\"id\": 749568003, \"type\": \"Route\", \"from\": \"SATi1wM6953xUoVovIP\", \"to\": \"Gz\"}, {\"id\": 434909132, \"type\": \"Route\", \"from\": \"SATi1wM6953xUoVovIP\", \"to\": \"SATi1wM6953xUoVovIP\"}, {\"id\": 1974787415, \"type\": \"Route\", \"from\": \"Gz\", \"to\": \"SATi1wM6953xUoVovIP\"}, {\"id\": 434909132, \"type\": \"Route\", \"from\": \"SATi1wM6953xUoVovIP\", \"to\": \"SATi1wM6953xUoVovIP\"}, {\"id\": 84618545, \"type\": \"Stop\", \"name\": \"Gz\"}, {\"id\": 29388557, \"type\": \"Route\", \"from\": \"Gz\", \"to\": \"Gz\"}, {\"id\": 1974787415, \"type\": \"Route\", \"from\": \"Gz\", \"to\": \"SATi1wM6953xUoVovIP\"}, {\"id\": 1974787415, \"type\": \"Route\", \"from\": \"Gz\", \"to\": \"SATi1wM6953xUoVovIP\"}, {\"id\": 29388557, \"type\": \"Route\", \"from\": \"Gz\", \"to\": \"Gz\"}, {\"id\": 116628399, \"type\": \"Map\"}]}";
    std::istringstream input(json);

    transport_catalogue::json_reader::ProcessRequestsLoad(input, std::cout, catalogue, router, handler, serializer);
}*/

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

void MakeBase() {
    transport_catalogue::TransportCatalogue catalogue;
    transport_catalogue::renderer::MapRenderer renderer;
    transport_catalogue::router::TransportRouter router(catalogue);
    transport_catalogue::serialization::Serializer serializer(catalogue, renderer, router);

    transport_catalogue::json_reader::MakeBaseLoad(std::cin, catalogue, renderer, router, serializer);
}

void ProcessRequests() {
    transport_catalogue::TransportCatalogue catalogue;
    transport_catalogue::renderer::MapRenderer renderer;
    transport_catalogue::router::TransportRouter router(catalogue);
    transport_catalogue::request_handler::RequestHandler handler(catalogue, renderer, router);
    transport_catalogue::serialization::Serializer serializer(catalogue, renderer, router);

    transport_catalogue::json_reader::ProcessRequestsLoad(std::cin, std::cout, catalogue, router, handler, serializer);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    if (mode == "make_base"sv) {
        MakeBase();
        //TestMakeBase();
    } else if (mode == "process_requests"sv) {
        ProcessRequests();
        //TestProcessRequests();
    } else {
        PrintUsage();
        return 1;
    }
}