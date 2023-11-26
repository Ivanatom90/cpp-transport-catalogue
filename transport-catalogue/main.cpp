#include "transport_catalogue.h"
#include "json_reader.h"
#include "domain.h"
#include "map_renderer.h"
#include "request_handler.h"
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>



using namespace std;
using namespace transport_catalogue;
using namespace transport_catalogue::detail;
using namespace transport_catalogue::detail::json;

int main() {

    std::ostringstream oss;
    //std::ofstream picture("out.svg");
    //std::ofstream text("test_out.txt");
    //std::ifstream infile("in.txt");
    Document doc = load(cin);
    JSONr readJSON(doc, cout);
    readJSON.ParseReq();
    readJSON.CreateTransportCataloge();    
    //TransportCatalog tc = std::move(readJSON.GetTransportCataloge());
    TransportCatalog tc = readJSON.GetTransportCataloge();
    request_handler::RequestHandler req_handler(tc);
    RenderSettings rs = std::move(readJSON.GetRenderSettings());
    BusRoutes& br = req_handler.GetBusesRoutes();
    MapRenderer map_renderer(std::move(rs),std::move(br));
    //map_renderer.PrintDocument(text);
    //map_renderer.PrintDocument(picture);
    map_renderer.PrintDocument(oss);
    std::string mapa;
    mapa = oss.str();
    readJSON.SetMap(mapa);
    readJSON.PrintAnswer();



    //readJSON.PrintAnswer();



    /*
     * Примерная структура программы:
     *
     * Считать JSON из stdin
     * Построить на его основе JSON базу данных транспортного справочника
     * Выполнить запросы к справочнику, находящиеся в массиве "stat_requests", построив JSON-массив
     * с ответами.
     * Вывести в stdout ответы в виде JSON
     */
}
