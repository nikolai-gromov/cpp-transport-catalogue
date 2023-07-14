#pragma once
#include "geo.h"

#include <string>
#include <vector>

namespace domain {

struct Stop {
    std::string name;
    geo::Coordinates coordinates;
};

struct Bus {
    std::string name;
    std::vector<const Stop*> stops;
    bool is_roundtrip;
};

struct BusStat {
    std::size_t stops_on_route = 0;
    std::size_t unique_stops = 0;
    double distance_coordinates = 0.;
    double distance = 0.;
    double curvature = 0.;
};

}