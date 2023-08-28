#include "stat_reader.h"

#include <iomanip>
#include <iostream>
#include <set>

namespace transport_catalogue {

namespace stat_reader {

using namespace std::string_literals;

void PrintBusesByStop(std::ostream& out, std::string_view name, const TransportCatalogue& catalog) {
    const auto found_buses = catalog.GetBusesByStop(name);
    if (catalog.FindStop(name) != nullptr) {
        if (found_buses.size() != 0) {
            out << "Stop "s << name << ": buses "s;
            for (auto it = found_buses.begin(); it != found_buses.end(); ++it) {
                if (std::next(it) != found_buses.end()) {
                    out << (*it) << " "s;
                } else {
                    out << (*it);
                }
            }
            out << std::endl;
        } else {
            out << "Stop "s << name << ": no buses"s << std::endl;
        }
    } else {
        out << "Stop "s << name << ": not found"s << std::endl;
    }
}

void PrintBusStat(std::ostream& out, std::string_view name, const TransportCatalogue& catalog) {
    if (catalog.FindBus(name) != nullptr) {
        const auto stat = catalog.GetBusStat(catalog.FindBus(name));
        out << "Bus "s << name << ": "s
            << stat.stops_on_route << " stops on route, "s
            << stat.unique_stops << " unique stops, "s
            << std::setprecision(6) << stat.distance << " route length, "s
            << std::setprecision(6) << stat.curvature << " curvature"s << std::endl;
    } else {
        out << "Bus "s << name << ": not found"s << std::endl;
    }
}

void ParsingCatalogRequests(std::istream& in, const TransportCatalogue& catalog) {
    int query_count;
    in >> query_count;
    in.ignore(1);
    std::string query;
    std::vector<std::string> queries;
    queries.reserve(query_count);
    for (int i = 0; i < query_count; ++i) {
        std::getline(in, query);
        queries.push_back(std::move(query));
    }
    for (auto query : queries) {
        auto pos_space = query.find(' ');
        std::string type_query = query.substr(0, pos_space);
        if (type_query == "Stop") {
            auto pos_space = query.find(' ');
            std::string stopname = query.substr((pos_space + 1), (query.size() - (pos_space + 1)));
            PrintBusesByStop(std::cout, stopname, catalog);
        }
        if (type_query == "Bus") {
            auto pos_space = query.find(' ');
            std::string busname = query.substr((pos_space + 1), (query.size() - (pos_space + 1)));
            PrintBusStat(std::cout, busname, catalog);
        }
    }
}

}  // namespace stat_reader
}  // namespace transport_catalogue