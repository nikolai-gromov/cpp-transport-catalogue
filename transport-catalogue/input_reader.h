#pragma once
#include "domain.h"
#include "transport_catalogue.h"

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace transport_catalogue {

namespace input_reader {

struct Queries {
    std::vector<std::string> bus;
    std::vector<std::string> stop;
};

std::string_view GetName(std::string_view query);

std::vector<std::string_view> SplitStopData(std::string_view str, char c);

void GetDistance(std::string_view stopname, std::vector<std::string_view> stop_data, TransportCatalogue& catalog);

void FillStop(std::string_view query, TransportCatalogue& catalog, bool flag = false);

std::vector<std::string_view> SplitBusData(std::string_view str, char c);

std::vector<std::string_view> GetStops(std::string_view query);

domain::Bus FillBus(std::string_view query, TransportCatalogue& catalog);

std::string_view GetQueryType(std::string_view query);

void Load(std::istream& in, TransportCatalogue& catalog);

} // namespace input_reader
} // namespace transport_catalogue