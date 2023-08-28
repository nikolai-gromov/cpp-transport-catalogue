#pragma once
#include "domain.h"

#include <deque>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace transport_catalogue {

namespace detail {

template<typename Type>
class HasherPair {
public:
    size_t operator()(std::pair<const Type*, const Type*> pr) const
    {
        static const size_t p = 37;
        return hasher_(pr.first) + p * hasher_(pr.second);
    }

private:
    std::hash<const Type*> hasher_;
};

} // namespace detail

using StopPtr = const domain::Stop*;
using BusPtr = const domain::Bus*;
using BusStat = const domain::BusStat;

class TransportCatalogue {
public:
    void AddStop(const domain::Stop& stop);
    void AddBus(const domain::Bus& bus);

    StopPtr FindStop(std::string_view name) const;
    BusPtr FindBus(std::string_view name) const;

    void SetDistanceBetweenStops(std::string_view stop_first, std::string_view stop_second, const double distance);

    double GetDistanceBetweenStops(StopPtr stop_first_ptr, StopPtr stop_second_ptr) const;

    BusStat GetBusStat(BusPtr bus) const;

    const std::set<std::string_view> GetBusesByStop(std::string_view name) const;

    size_t GetStopsCount() const;

    const std::unordered_map<std::string_view, StopPtr>& GetStopNameToStop() const;
    const std::unordered_map<std::string_view, BusPtr>& GetBusNameToBus() const;


private:
    std::deque<domain::Stop> stops_;
    std::unordered_map<std::string_view, StopPtr> stopname_to_stop_;
    std::deque<domain::Bus> buses_;
    std::unordered_map<std::string_view, BusPtr> busname_to_bus_;
    std::unordered_map<std::pair<StopPtr, StopPtr>, double, detail::HasherPair<domain::Stop>> distance_between_stops_;
};

}  // namespace transport_catalogue