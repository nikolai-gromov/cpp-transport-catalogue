#include "input_reader.h"
#include "transport_catalogue.h"

int main() {
    using namespace transport_catalogue;
    using namespace query;

    TransportCatalogue catalog;
    InputReader reader;
    reader.ParsingInputData();
    reader.ParsingInputData();
    reader.Load(catalog);
    return 0;
}