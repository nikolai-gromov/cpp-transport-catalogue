#pragma once
#include "domain.h"
#include "transport_catalogue.h"

#include <string_view>

namespace transport_catalogue {

namespace stat_reader {

void PrintBusesByStop(std::ostream& out, std::string_view name, const TransportCatalogue& catalog);

void PrintBusStat(std::ostream& out, std::string_view name, const TransportCatalogue& catalog);

void ParsingCatalogRequests(std::istream& input, const TransportCatalogue& catalog);

}  // namespace stat_reader
}  // namespace transport_catalogue