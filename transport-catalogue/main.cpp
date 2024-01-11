#include "transport_catalogue.h"
#include "json_reader.h"
#include "domain.h"
#include "map_renderer.h"
#include "request_handler.h"
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <memory>

#include "json_builder.h"
#include "transport_router.h"
#include "log_duration.h"



using namespace std;
using namespace transport_catalogue;
using namespace transport_catalogue::detail;
using namespace transport_catalogue::detail::json;
using transport_catalogue::detail::router::TransportRouter;

int main() {

    std::ostringstream oss;
    //std::ofstream picture("out.svg");
    //std::ofstream out("out.txt");
    //std::ofstream text("test_out.txt");
    //std::ifstream infile("in.txt");
    Document doc = Load(cin);
    JSONr readJSON(doc, cout);
    readJSON.ParseReq();
    readJSON.CreateTransportCataloge();    
    //TransportCatalog tc = std::move(readJSON.GetTransportCataloge());
    TransportCatalog tc = readJSON.GetTransportCataloge();
    request_handler::RequestHandler req_handler(tc);
    RenderSettings rs = std::move(readJSON.GetRenderSettings());
    BusRoutes& br = req_handler.GetBusesRoutes();
    MapRenderer map_renderer(std::move(rs),br);
    //map_renderer.PrintDocument(text);
    //map_renderer.PrintDocument(picture);
    map_renderer.PrintDocument(oss);
    std::string mapa;
    mapa = oss.str();
    readJSON.SetMap(mapa);   
    readJSON.SetTransportRouter(std::unique_ptr<TransportRouter>(new TransportRouter(tc)));
    readJSON.CreateAnswer();

    //readJSON.PrintAnswer();
    return 0;

}
