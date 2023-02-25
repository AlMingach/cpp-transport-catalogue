#include <cassert>
#include <chrono>
#include <sstream>
#include <string_view>

#include "json.h"
#include "json_reader.h"

using namespace json_reader;
using namespace std::literals;

int main() {
    transport_catalogue::TransportCatalogue catalogue;
    renderer::MapRenderer render;
    Json_TC data(std::cin);
    data.LoadCatalogueData(catalogue);
    data.LoadRenderSetings(render);
    TransportCatalogueHandler resul(catalogue, render);
    data.PrintCatalogueStatRequests(resul, std::cout);
}
