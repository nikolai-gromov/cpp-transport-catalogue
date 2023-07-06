#pragma once
#include "geo.h"

#include <deque>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace transport_catalogue
{
namespace detail
{
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
}

struct Stop {
    std::string name;
    geo::Coordinates coordinates;
};

struct Bus {
    std::string name;
    std::vector<const Stop*> stops;
};

struct BusInfo {
    std::size_t stops_on_route = 0;
    std::size_t unique_stops = 0;
    double distance_coordinates = 0.;
    double distance = 0.;
    double curvature = 0.;
};

class TransportCatalogue {
public:
    void AddStop(Stop stop);

    const Stop* FindStop(std::string_view name) const;

    void AddBus(Bus bus);

    const Bus* FindBus(std::string_view name) const;

    std::set<std::string_view> GetBusesByStop(std::string_view name) const;

    void SetDistanceBetweenStops(std::string_view name_first, const double distance, std::string_view name_second);

    double GetDistanceBetweenStops(const Stop* stop_first_ptr, const Stop* stop_second_ptr) const;

    BusInfo GetBusInfo(const Bus* bus) const;

private:
    std::deque<Stop> stops_;
    std::unordered_map<std::string_view, const Stop*> stopname_to_stop_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, const Bus*> busname_to_bus_;
    std::unordered_map<std::pair<const Stop*, const Stop*>, double, detail::HasherPair<Stop>> distance_between_stops_;
};
}