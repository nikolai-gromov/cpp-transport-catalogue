#pragma once
#include "transport_catalogue.h"

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace transport_catalogue
{
namespace query
{
inline std::vector<std::string_view> Split(std::string_view str, char c);

enum class QueryType {
    Stop,
    Bus
};

struct CatalogQueries {
    QueryType type;
    std::string name;
    std::pair<std::string_view, std::string_view> coordinates;
    std::vector<std::string_view> list_of_stops;
    std::vector<std::pair<std::uint64_t, std::string_view>> distance_to_stop;
    RouteType route_type = RouteType::Usual;
    std::string raw_query;
    std::string pro_query;

    std::vector<std::string_view> ParsingRouteTypeHandling(std::vector<std::string_view> input_vec);

    void ParsingQueryTypeHandling(std::string input);
};

class InputReader {
public:
    void ParsingInputData();
    void Load(TransportCatalogue& catalog);
    void LoadData(TransportCatalogue& catalog, CatalogQueries queries, bool flag = false);

private:
    std::vector<CatalogQueries> queries_;
};

}
}