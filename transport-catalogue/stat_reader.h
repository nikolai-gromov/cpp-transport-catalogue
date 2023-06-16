#pragma once
#include "input_reader.h"
#include "transport_catalogue.h"

#include <string_view>

namespace transport_catalogue
{
namespace output
{
void OutputStopsData(TransportCatalogue& catalog, std::string_view name);

void OutputRouteData(TransportCatalogue& catalog, std::string_view name);

void OutputData(TransportCatalogue& catalog, query::CatalogQueries queries);
}
}