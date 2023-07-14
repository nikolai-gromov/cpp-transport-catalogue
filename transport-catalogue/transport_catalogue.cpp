#include "transport_catalogue.h"

#include <algorithm>

namespace transport_catalogue {

void TransportCatalogue::AddStop(domain::Stop stop) {
    stops_.push_back(stop);
    stopname_to_stop_[stops_.back().name] = &stops_.back();
}

StopPtr TransportCatalogue::FindStop(std::string_view name) const {
    if (auto is_stop = stopname_to_stop_.find(name); is_stop != stopname_to_stop_.end()) {
        return stopname_to_stop_.at(name);
    }
    return nullptr;
}

void TransportCatalogue::AddBus(domain::Bus bus) {
    buses_.push_back(bus);
    busname_to_bus_[buses_.back().name] = &buses_.back();
}

BusPtr TransportCatalogue::FindBus(std::string_view name) const {
    if (auto is_bus = busname_to_bus_.find(name); is_bus != busname_to_bus_.end()) {
        return busname_to_bus_.at(name);
    }
    return nullptr;
}

const std::unordered_map<std::string_view, BusPtr>& TransportCatalogue::GetBuses() const {
    return  busname_to_bus_;
}

const std::set<std::string_view> TransportCatalogue::GetBusesByStop(std::string_view name) const {
    std::set<std::string_view> result;
    if (nullptr != FindStop(name)) {
        std::for_each(
                    busname_to_bus_.begin(), busname_to_bus_.end(),
                    [&](const auto buses) {
                        for (const auto stop : buses.second->stops) {
                            if (name == stop->name) {
                                result.insert(buses.second->name);
                                break;
                            }
                        }
                    });
    }
    return result;
}

void TransportCatalogue::SetDistanceBetweenStops(std::string_view name_first, double distance, std::string_view name_second) {
    auto stop_first_ptr = FindStop(name_first);
    auto stop_second_ptr = FindStop(name_second);
    if (stop_first_ptr != nullptr && stop_second_ptr != nullptr) {
        distance_between_stops_[{ stop_first_ptr, stop_second_ptr }] = distance;
        distance_between_stops_.try_emplace({ stop_second_ptr, stop_first_ptr }, distance);
    }
}

double TransportCatalogue::GetDistanceBetweenStops(StopPtr stop_first_ptr, StopPtr stop_second_ptr) const {
    return distance_between_stops_.at({ stop_first_ptr, stop_second_ptr });
}

BusStat TransportCatalogue::GetBusStat(BusPtr bus) const {
    domain::BusStat data;
    data.stops_on_route = bus->stops.size();
    auto tmp = bus->stops;
    std::sort(tmp.begin(), tmp.end());
    auto it = std::unique(tmp.begin(), tmp.end());
    tmp.resize(std::distance(tmp.begin(), it));
    data.unique_stops = tmp.size();
    for (int i = 0; i < (static_cast<int>(bus->stops.size()) - 1); ++i) {
        data.distance_coordinates += ComputeDistance(bus->stops[i]->coordinates, bus->stops[i + 1]->coordinates);
        data.distance += GetDistanceBetweenStops(bus->stops[i], bus->stops[i + 1]);
    }
    data.curvature = data.distance / data.distance_coordinates;
    return data;
}

} // namespace transport_catalogue