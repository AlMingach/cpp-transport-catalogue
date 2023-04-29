#include <fstream>
#include <iostream>
#include <string_view>

#include "json_reader.h"

using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

     transport_catalogue::TransportCatalogue catalogue;
     renderer::MapRenderer render;
     transport_router::TransportRouter router;
     serialize::Serializator proto_catalogue;
     TransportCatalogueHandler handler(catalogue, render, router); // Создание фасада

    if (mode == "make_base"sv) {
        json_reader::Json_TC data(std::cin);
        data.LoadCatalogueData(catalogue); // выгрузка данных из json_reader в каталог
        data.LoadRenderSetings(render); // выгрузка данных из json_reader в MapRender
        router.AddRouterSetting(data.GetRouterSettings());
        router.InicializeGraph(catalogue);
        proto_catalogue.SetSerializeSettings(data.GetSerializeSettings());
        proto_catalogue.AddTransportCatalogue(catalogue);
        proto_catalogue.AddRenderSettings(render);
        proto_catalogue.SetProtoTransportRouter(router);
        proto_catalogue.Serialize();
        
    } else if (mode == "process_requests"sv) {
        json_reader::Json_TC data(std::cin);
       proto_catalogue.SetSerializeSettings(data.GetSerializeSettings());
       proto_catalogue.Deserialize(catalogue, render, router);

        data.PrintCatalogueStatRequests(handler, std::cout);

    } else {
       PrintUsage();
       return 1;
    }
}
