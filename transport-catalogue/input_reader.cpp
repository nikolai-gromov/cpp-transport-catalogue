#include "input_reader.h"

#include <algorithm>

namespace transport_catalogue {

namespace input_reader {

std::string_view GetName(std::string_view query) {
    auto begin_name = query.find(' ') + 1;
    auto end_name = query.find(':') - begin_name;
    return query.substr(begin_name, end_name);
}

std::vector<std::string_view> SplitStopData(std::string_view str, char c) {
    std::vector<std::string_view> result;
    int64_t pos_end = str.npos;
    while (true) {
        int64_t pos_c = str.find(c);
        result.push_back(str.substr(0, pos_c));
        if (pos_c != pos_end) {
            str.remove_prefix(pos_c + 2);
        } else {
            break;
        }
    }
    return result;
}

void GetDistance(std::string_view stopname, std::vector<std::string_view> stop_data, TransportCatalogue& catalog) {
    std::string_view distance, stop;
    for (auto i = 0u; i < stop_data.size(); ++i) {
        auto pos_m = stop_data[i].find('m');
        distance = stop_data[i].substr(0, pos_m);
        auto pos_t = stop_data[i].find('t');
        stop = stop_data[i].substr((pos_t + 3), ((stop_data[i].size()) - (pos_t + 3)));
        catalog.SetDistanceBetweenStops(stopname, std::stod(std::string(distance)), stop);
    }
    stop_data.clear();
}

void FillStop(std::string_view query, TransportCatalogue& catalog, bool flag) {
    domain::Stop result;
    if (flag != true) {
        std::string_view stopname = GetName(query);
        result.name = std::string(stopname);
        query.remove_prefix((stopname.size() + 7));
        auto stop_data = SplitStopData(query, ',');
        std::string_view latitude, longitude;
        latitude = stop_data[0];
        stop_data.erase(stop_data.begin());
        longitude = stop_data[0];
        stop_data.erase(stop_data.begin());
        result.coordinates.lat = std::stod(std::string(latitude));
        result.coordinates.lng = std::stod(std::string(longitude));
        catalog.AddStop(result);
    } else {
        std::string_view stopname = GetName(query);
        query.remove_prefix((stopname.size() + 7));
        auto stop_data = SplitStopData(query, ',');
        if (2u < stop_data.size()) {
            stop_data.erase(stop_data.begin());
            stop_data.erase(stop_data.begin());
            GetDistance(stopname, stop_data, catalog);
        }
    }
}

std::vector<std::string_view> SplitBusData(std::string_view str, char c) {
    std::vector<std::string_view> result;
    int64_t pos_end = str.npos;
    while (true) {
        int64_t pos_c = str.find(c);
        result.push_back(str.substr(0, pos_c - 1));
        if (pos_c != pos_end) {
            str.remove_prefix(pos_c + 2);
        } else {
            break;
        }
    }
    return result;
}

std::vector<std::string_view> GetStops(std::string_view query) {
    std::vector<std::string_view> result;
    if (auto pos = query.find('-'); pos != std::string::npos) {
        result = SplitBusData(query, '-');
        for (int i = (static_cast<int>(result.size()) - 2); i >= 0; --i) {
            result.push_back(result[i]);
        }
    } else {
        result = SplitBusData(query, '>');
    }
    return result;
}

domain::Bus FillBus(std::string_view query, TransportCatalogue& catalog) {
    domain::Bus result;
    std::string_view busname = GetName(query);
    result.name = std::string(busname);
    query.remove_prefix((busname.size() + 6));
    for (std::string_view stop : GetStops(query)) {
        result.stops.push_back(catalog.FindStop(stop));
    }
    return result;
}

std::string_view GetQueryType(std::string_view query) {
    auto end_type = query.find(' ');
    return query.substr(0, end_type);
}

void Load(std::istream& in, TransportCatalogue& catalog) {
    int query_count;
    in >> query_count;
    in.ignore(1);
    std::string query;
    Queries queries;
    for (int i = 0; i < query_count; ++i) {
        std::getline(in, query);
        std::string_view type_query = GetQueryType(query);
        queries.bus.reserve(query_count);
        queries.stop.reserve(query_count);
        if (type_query == "Bus") {
            queries.bus.push_back(std::move(query));
        } else {
            queries.stop.push_back(std::move(query));
        }
    }
    for (auto query : queries.stop) {
        FillStop(query, catalog);
    }
    for (auto query : queries.stop) {
        FillStop(query, catalog, true);
    }
    queries.stop.clear();
    for (auto query : queries.bus) {
        catalog.AddBus(FillBus(query, catalog));
    }
    queries.bus.clear();
}

} // namespace input_reader
} // namespace transport_catalogue