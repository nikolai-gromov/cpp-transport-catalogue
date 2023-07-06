#pragma once
#include "transport_catalogue.h"

#include <string_view>

namespace transport_catalogue
{
namespace stat_reader
{
void PrintStopsData(std::ostream& output, std::string_view name, TransportCatalogue& catalog);

void PrintRouteData(std::ostream& output, std::string_view name, TransportCatalogue& catalog);

void ParsingCatalogRequests(std::istream& input, TransportCatalogue& catalog);
}
}