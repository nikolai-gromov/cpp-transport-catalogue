#include "input_reader.h"
#include "transport_catalogue.h"
#include "stat_reader.h"

int main() {
    transport_catalogue::TransportCatalogue catalog;
    transport_catalogue::input_reader::Load(std::cin, catalog);
    transport_catalogue::stat_reader::ParsingCatalogRequests(std::cin, catalog);
    return 0;
}