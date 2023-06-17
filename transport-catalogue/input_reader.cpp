#include "input_reader.h"
#include "stat_reader.h"

#include <algorithm>
#include <unordered_map>

namespace transport_catalogue
{
namespace query
{
inline std::vector<std::string_view> Split(std::string_view str, char c) {
    std::vector<std::string_view> result;
    auto pos = 0;
    while (true) {
        auto pos_c = str.find(c, pos);
        result.push_back(pos_c == str.npos ? str.substr(pos) : str.substr(pos, pos_c - pos));
        if (pos_c == str.npos) {
            break;
        } else {
            pos = pos_c + 1;
        }
    }
    return result;
}

std::vector<std::string_view> CatalogQueries::ParsingRouteTypeHandling(std::vector<std::string_view> input_vec) {
    std::vector<std::string_view> tmp, result;
    if (input_vec.size() > 3) {
        if (pro_query.find('-') != std::string::npos) {
            route_type = RouteType::Usual;
            tmp = Split(pro_query, '-');
        }
        if (pro_query.find('>') != std::string::npos) {
            route_type = RouteType::Ring;
            tmp = Split(pro_query, '>');
        }
    }
    for (size_t i = 0; i < tmp.size(); ++i) {
        while (tmp[i].front() == ' ') tmp[i].remove_prefix(1);
        while (tmp[i].back() == ' ') tmp[i].remove_suffix(1);
        result.push_back(tmp[i]);
    }
    return result;
}

void CatalogQueries::ParsingQueryTypeHandling(std::string input) {
    raw_query = std::move(input);
    auto input_vec = Split(raw_query, ' ');
    auto it_begin_type = raw_query.find_first_not_of(' ');
    auto it_end_type  = raw_query.find(' ', it_begin_type);

    static const std::unordered_map<std::string, QueryType> types_of_query = {{ "Stop", QueryType::Stop }, { "Bus", QueryType::Bus }};
    std::string type_query = { raw_query.begin() + it_begin_type, raw_query.begin() + it_end_type };

    switch (types_of_query.at(type_query))
    {
    case QueryType::Stop:
        type = QueryType::Stop;
        if (auto pos = raw_query.find(':'); pos != std::string::npos) {
            for (std::size_t i = it_end_type; i < pos; ++i) {
                name += raw_query[i];
            }
            while (name.front() == ' ') name.erase(name.begin());
            pro_query = raw_query.substr((pos + 2), (raw_query.size() - (pos + 2)));
            auto tmp = Split(pro_query, ',');
            std::string_view str, stop;
            std::uint64_t distance;
            if (tmp.size() > 2) {
                while (tmp[1].front() == ' ') tmp[1].remove_prefix(1);
                coordinates = { tmp[0], tmp[1] };
                for (std::size_t i = 2; i != tmp.size(); ++i) {
                    while (tmp[i].front() == ' ') tmp[i].remove_prefix(1);
                    auto pos_m = tmp[i].find('m');
                    str = tmp[i].substr(0, pos_m);
                    distance = std::stoi(static_cast<std::string>(str));
                    auto pos_t = tmp[i].find('t');
                    stop = tmp[i].substr((pos_t + 2), tmp[i].size());
                    while (stop.front() == ' ') stop.remove_prefix(1);
                    distance_to_stop.push_back({ distance, stop });
                }
            } else {
                while (tmp[1].front() == ' ') tmp[1].remove_prefix(1);
                coordinates = { tmp[0], tmp[1] };
            }
        } else {
            for (size_t i = it_end_type; i < raw_query.size(); ++i) {
                name += raw_query[i];
            }
            while (name.front() == ' ') name.erase(name.begin());
            while (name.back() == ' ') name.erase(name.end() - 1);
            break;
        }
        break;
    case QueryType::Bus:
        type = QueryType::Bus;
        if (auto pos = raw_query.find(':'); pos != std::string::npos) {
            for (std::size_t i = it_end_type; i < pos; ++i) {
                name += raw_query[i];
            }
            while (name.front() == ' ') name.erase(name.begin());
            pro_query = raw_query.substr((pos + 2), (raw_query.size() - (pos + 2)));
            list_of_stops = ParsingRouteTypeHandling(input_vec);
        } else {
            for (std::size_t i = it_end_type; i < raw_query.size(); ++i) {
                name += raw_query[i];
            }
            while (name.front() == ' ') name.erase(name.begin());
            while (name.back() == ' ') name.erase(name.end() - 1);
            break;
        }
        break;
    }
}

void InputReader::ParsingInputData() {
    int query_count;
    std::cin >> query_count;
    std::cin.ignore();
    std::string query;
    for (int i = 0; i < query_count; ++i) {
        std::getline(std::cin, query);
        CatalogQueries queries;
        queries.ParsingQueryTypeHandling(std::move(query));
        queries_.push_back(std::move(queries));
    }
}

void InputReader::Load(TransportCatalogue& catalog) {
    auto it_pro = std::partition(queries_.begin(), queries_.end(), [](CatalogQueries queries)
    {
        return !queries.pro_query.empty();
    });
    auto it_stops = std::partition(queries_.begin(), it_pro, [](CatalogQueries queries)
    {
        return queries.type == QueryType::Stop;
    });
    for (auto it = queries_.begin(); it != it_stops; ++it) {
        InputReader::LoadData(catalog, *it, true);
    }
    for (auto it = queries_.begin(); it != it_stops; ++it) {
        InputReader::LoadData(catalog, *it);
    }
    for (auto it = it_stops; it != it_pro; ++it) {
        InputReader::LoadData(catalog, *it, true);
    }
    for (auto it = it_pro; it != queries_.end(); ++it) {
        InputReader::LoadData(catalog, *it, true);
    }
}

void InputReader::LoadData(TransportCatalogue& catalog, CatalogQueries queries, bool flag) {
    switch (queries.type)
    {
    case QueryType::Stop:
        if (queries.coordinates != std::pair<std::string_view, std::string_view>()) {
            std::string lat = {queries.coordinates.first.begin(), queries.coordinates.first.end()};
            std::string lon = {queries.coordinates.second.begin(), queries.coordinates.second.end()};
            if (flag == true) {
                catalog.AddStop(queries.name, std::stod(lat), std::stod(lon));
            } else {
                if (!queries.distance_to_stop.empty()) {
                    for (auto [distance, stop] : queries.distance_to_stop) {
                        catalog.SetDistanceBetweenStops(queries.name, distance, stop);
                    }
                }
            }
        } else {
            output::OutputStopsData(catalog, queries.name);
        }
        break;
    case QueryType::Bus:
        if (!queries.list_of_stops.empty()) {
            catalog.AddBus(queries.name, queries.route_type, queries.list_of_stops);
        } else {
            output::OutputRouteData(catalog, queries.name);
        }
        break;
    }
}
}
}