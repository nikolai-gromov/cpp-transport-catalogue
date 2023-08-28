#include "transport_catalogue.h"

#include <algorithm>

namespace transport_catalogue {

void TransportCatalogue::AddStop(const domain::Stop& stop) {
    stops_.push_back(stop);
    stopname_to_stop_[stops_.back().name] = &stops_.back();
}
void TransportCatalogue::AddBus(const domain::Bus& bus) {
    buses_.push_back(bus);
    busname_to_bus_[buses_.back().name] = &buses_.back();
}

StopPtr TransportCatalogue::FindStop(std::string_view name) const {
    if (auto is_stop = stopname_to_stop_.find(name); is_stop != stopname_to_stop_.end()) {
        return stopname_to_stop_.at(name);
    }
    return nullptr;
}
BusPtr TransportCatalogue::FindBus(std::string_view name) const {
    if (auto is_bus = busname_to_bus_.find(name); is_bus != busname_to_bus_.end()) {
        return busname_to_bus_.at(name);
    }
    return nullptr;
}

void TransportCatalogue::SetDistanceBetweenStops(std::string_view stop_first, std::string_view stop_second, const double distance) {
    auto stop_first_ptr = FindStop(stop_first);
    auto stop_second_ptr = FindStop(stop_second);
    if (stop_first_ptr != nullptr && stop_second_ptr != nullptr) {
        distance_between_stops_[{ stop_first_ptr, stop_second_ptr }] = distance;
        distance_between_stops_.try_emplace({ stop_second_ptr, stop_first_ptr }, distance);
    }
}

double TransportCatalogue::GetDistanceBetweenStops(StopPtr stop_first_ptr, StopPtr stop_second_ptr) const {
    return distance_between_stops_.at({ stop_first_ptr, stop_second_ptr });
}

BusStat TransportCatalogue::GetBusStat(BusPtr bus) const {
    domain::BusStat stat;
    stat.stops_on_route = bus->stops.size();
    auto tmp = bus->stops;
    std::sort(tmp.begin(), tmp.end());
    auto it = std::unique(tmp.begin(), tmp.end());
    tmp.resize(std::distance(tmp.begin(), it));
    stat.unique_stops = tmp.size();
    for (int i = 0; i < (static_cast<int>(bus->stops.size()) - 1); ++i) {
        stat.distance_coordinates += ComputeDistance(bus->stops[i]->coordinates, bus->stops[i + 1]->coordinates);
        stat.distance += GetDistanceBetweenStops(bus->stops[i], bus->stops[i + 1]);
    }
    stat.curvature = stat.distance / stat.distance_coordinates;
    return stat;
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

size_t TransportCatalogue::GetStopsCount() const {
    return stops_.size();
}

const std::unordered_map<std::string_view, StopPtr>& TransportCatalogue::GetStopNameToStop() const {
    return  stopname_to_stop_;
}
const std::unordered_map<std::string_view, BusPtr>& TransportCatalogue::GetBusNameToBus() const {
    return  busname_to_bus_;
}

}  // namespace transport_catalogue