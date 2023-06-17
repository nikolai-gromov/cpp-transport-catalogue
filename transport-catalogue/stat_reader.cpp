#include "stat_reader.h"

#include <iomanip>
#include <iostream>
#include <set>

namespace transport_catalogue
{
namespace output
{
using namespace std::string_literals;

void OutputStopsData(TransportCatalogue& catalog, std::string_view name) {
    std::set<std::string_view> found_buses = catalog.GetBusesByStop(name);
    if (catalog.FindStop(name) != nullptr) {
        if (found_buses.size() != 0) {
            std::cout << "Stop "s << name << ": buses "s;
            for (auto it = found_buses.begin(); it != found_buses.end(); ++it) {
                if (std::next(it) != found_buses.end()) {
                    std::cout << *it << " "s;
                } else {
                    std::cout << *it;
                }
            }
            std::cout << std::endl;
        } else {
            std::cout << "Stop "s << name << ": no buses"s << std::endl;
        }
    } else {
        std::cout << "Stop "s << name << ": not found"s << std::endl;
    }
}

void OutputRouteData(TransportCatalogue& catalog, std::string_view name) {
    if (catalog.FindBus(name) != nullptr) {
        BusInfo data = catalog.GetBusInfo(catalog.FindBus(name));
        std::cout << "Bus "s << name << ": "s << data.stops_on_route
            << " stops on route, "s << data.unique_stops
            << " unique stops, "s << std::setprecision(6)
            << data.distance << " route length, "s
            << std::setprecision(6) << data.curvature
            << " curvature"s << std::endl;
    } else {
        std::cout << "Bus "s << name << ": not found"s << std::endl;
    }
}
}
}