#include "stat_reader.h"

#include <iomanip>
#include <iostream>
#include <set>

namespace transport_catalogue
{
namespace stat_reader
{
using namespace std::string_literals;

void PrintStopsData(std::ostream& output, std::string_view name, TransportCatalogue& catalog) {
    std::set<std::string_view> found_buses = catalog.GetBusesByStop(name);
    if (catalog.FindStop(name) != nullptr) {
        if (found_buses.size() != 0) {
            output << "Stop "s << name << ": buses "s;
            for (auto it = found_buses.begin(); it != found_buses.end(); ++it) {
                if (std::next(it) != found_buses.end()) {
                    output << *it << " "s;
                } else {
                    output << *it;
                }
            }
            output << std::endl;
        } else {
            output << "Stop "s << name << ": no buses"s << std::endl;
        }
    } else {
        output << "Stop "s << name << ": not found"s << std::endl;
    }
}

void PrintRouteData(std::ostream& output, std::string_view name, TransportCatalogue& catalog) {
    if (catalog.FindBus(name) != nullptr) {
        BusInfo data = catalog.GetBusInfo(catalog.FindBus(name));
        output << "Bus "s << name << ": "s
            << data.stops_on_route << " stops on route, "s
            << data.unique_stops << " unique stops, "s
            << std::setprecision(6) << data.distance << " route length, "s
            << std::setprecision(6) << data.curvature << " curvature"s << std::endl;
    } else {
        output << "Bus "s << name << ": not found"s << std::endl;
    }
}

void ParsingCatalogRequests(std::istream& input, TransportCatalogue& catalog) {
    int query_count;
    input >> query_count;
    input.ignore(1);
    std::string query;
    std::vector<std::string> queries;
    queries.reserve(query_count);
    for (int i = 0; i < query_count; ++i) {
        std::getline(input, query);
        queries.push_back(std::move(query));
    }
    for (auto query : queries) {
        auto pos_space = query.find(' ');
        std::string type_query = query.substr(0, pos_space);
        if (type_query == "Stop") {
            auto pos_space = query.find(' ');
            std::string stopname = query.substr((pos_space + 1), (query.size() - (pos_space + 1)));
            PrintStopsData(std::cout, stopname, catalog);
        }
        if (type_query == "Bus") {
            auto pos_space = query.find(' ');
            std::string busname = query.substr((pos_space + 1), (query.size() - (pos_space + 1)));
            PrintRouteData(std::cout, busname, catalog);
        }
    }
}
}
}