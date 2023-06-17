#include "transport_catalogue.h"

#include <algorithm>

namespace transport_catalogue
{
void TransportCatalogue::AddStop(std::string_view name, double latitude, double longitude) {
    stops_.push_back({{ name.begin(), name.end() }, latitude, longitude});
    stopname_to_stop_[stops_.back().name] = &stops_.back();
}

const Stop* TransportCatalogue::FindStop(std::string_view name) {
    if (auto is_stop = stopname_to_stop_.find(name); is_stop != stopname_to_stop_.end()) {
        return stopname_to_stop_.at(name);
    }
    return nullptr;
}

void TransportCatalogue::AddBus(std::string_view name, RouteType route, const std::vector<std::string_view>& stops) {
    Bus result;
    result.name = { name.begin(), name.end() };
    for (auto& stop : stops) {
        auto found_stop = FindStop(stop);
        if (found_stop != nullptr) {
            result.stops.push_back(found_stop);
        }
    }
    if (route == RouteType::Usual) {
        auto tmp = result.stops;
        for (int i = tmp.size() - 2; i >= 0; --i) {
            result.stops.push_back(result.stops[i]);
        }
    }
    buses_.push_back(std::move(result));
    busname_to_bus_[buses_.back().name] = &buses_.back();
    for (std::string_view name : stops) {
        auto found_stop = FindStop(name);
        if (found_stop != nullptr) {
            buses_by_stop_[found_stop].insert(buses_.back().name);
        }
    }
}

const Bus* TransportCatalogue::FindBus(std::string_view name) {
    if (auto is_bus = busname_to_bus_.find(name); is_bus != busname_to_bus_.end()) {
        return busname_to_bus_.at(name);
    }
    return nullptr;
}

std::set<std::string_view> TransportCatalogue::GetBusesByStop(std::string_view name) {
    std::set<std::string_view> empty_set;
    if (auto found_stop = FindStop(name); found_stop != nullptr) {
        if (auto is_buses = buses_by_stop_.find(found_stop); is_buses != buses_by_stop_.end()) {
            return buses_by_stop_.at(found_stop);
        }
    }
    return empty_set;
}

void TransportCatalogue::SetDistanceBetweenStops(std::string_view name_first, const std::uint64_t distance, std::string_view name_second) {
    auto stop_first_ptr = FindStop(name_first);
    auto stop_second_ptr = FindStop(name_second);
    if (stop_first_ptr != nullptr && stop_second_ptr != nullptr) {
        distance_between_stops_[{ stop_first_ptr, stop_second_ptr }] = distance;
    }
}

std::uint64_t TransportCatalogue::GetDistanceBetweenStops(const Stop* stop_first_ptr, const Stop* stop_second_ptr) {
    std::uint64_t result = 0;
    if (auto is_distance = distance_between_stops_.find({ stop_first_ptr, stop_second_ptr });
                is_distance != distance_between_stops_.end()) {
        result = distance_between_stops_.at({ stop_first_ptr, stop_second_ptr } );
    } else if (auto is_distance = distance_between_stops_.find({ stop_second_ptr, stop_first_ptr });
                        is_distance != distance_between_stops_.end()) {
        result = distance_between_stops_.at({ stop_second_ptr, stop_first_ptr } );
    }
    return result;
}

BusInfo TransportCatalogue::GetBusInfo(const Bus* bus) {
    BusInfo data;
    data.stops_on_route = bus->stops.size();
    auto tmp = bus->stops;
    std::sort(tmp.begin(), tmp.end());
    auto it = std::unique(tmp.begin(), tmp.end());
    tmp.resize(std::distance(tmp.begin(), it));
    data.unique_stops = tmp.size();
    for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
        data.distance_coordinates += ComputeDistance(bus->stops[i]->coordinates, bus->stops[i + 1]->coordinates);
        data.distance += GetDistanceBetweenStops(bus->stops[i], bus->stops[i + 1]);
    }
    data.curvature = data.distance / data.distance_coordinates;
    return data;
}
}