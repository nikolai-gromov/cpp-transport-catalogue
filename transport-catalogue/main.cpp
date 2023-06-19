#include "input_reader.h"
#include "transport_catalogue.h"
#include "stat_reader.h"

int main() {
    transport_catalogue::TransportCatalogue catalog;
    transport_catalogue::input_reader::ReaderLoader readerloader;
    readerloader.ParsingRequestsForFillingInTheCatalog(std::cin);
    readerloader.Load(catalog);
    transport_catalogue::stat_reader::ParsingCatalogRequests(catalog, std::cin);
    return 0;
}