#pragma once
#include "transport_catalogue.h"

#include <string_view>

namespace transport_catalogue
{
namespace stat_reader
{
void OutputStopsData(TransportCatalogue& catalog, std::string_view name);

void OutputRouteData(TransportCatalogue& catalog, std::string_view name);

void ParsingCatalogRequests(TransportCatalogue& catalog, std::istream& input);
}
}