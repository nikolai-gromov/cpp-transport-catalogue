#pragma once
#include "geo.h"

#include <string>
#include <string_view>
#include <vector>
#include <deque>
#include <set>
#include <unordered_map>

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

enum class RouteType {
    Usual,
    Ring
};

struct Stop {
    std::string name;
    geo::Coordinates coordinates;
    double latitude = 0.;
    double longitude = 0.;
};

struct Bus {
    std::string name;
    std::vector<const Stop*> stops;
};

struct BusInfo {
    std::size_t stops_on_route = 0;
    std::size_t unique_stops = 0;
    double distance_coordinates = 0.;
    std::uint64_t distance = 0;
    double curvature = 0.;
};

class TransportCatalogue {
public:
    void AddStop(std::string_view name, double latitude, double longitude);

    const Stop* FindStop(std::string_view name);

    void AddBus(std::string_view name, RouteType route, const std::vector<std::string_view>& stops);

    const Bus* FindBus(std::string_view name);

    std::set<std::string_view> GetBusesByStop(std::string_view name);

    void SetDistanceBetweenStops(std::string_view name_first, const std::uint64_t distance, std::string_view name_second);

    std::uint64_t GetDistanceBetweenStops(const Stop* stop_first_ptr, const Stop* stop_second_ptr);

    BusInfo GetBusInfo(const Bus* bus);

private:
    std::deque<Stop> stops_;
    std::unordered_map<std::string_view, const Stop*> stopname_to_stop_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, const Bus*> busname_to_bus_;
    std::unordered_map<const Stop*, std::set<std::string_view>> buses_by_stop_;
    std::unordered_map<std::pair<const Stop*, const Stop*>, uint64_t, detail::HasherPair<Stop>> distance_between_stops_;
};
}