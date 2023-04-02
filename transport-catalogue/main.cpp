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
    Json_TC data(std::cin); // загрузка данных в json_reader
    data.LoadCatalogueData(catalogue); // выгрузка данных из json_reader в каталог
    data.LoadRenderSetings(render); // выгрузка данных из json_reader в MapRender
    transport_router::TransportRouter router(catalogue, data.GetRouterSettings());
    TransportCatalogueHandler resul(catalogue, render, router); // Создание фасада
    data.PrintCatalogueStatRequests(resul, std::cout); // работа с фасадом для Json
}
