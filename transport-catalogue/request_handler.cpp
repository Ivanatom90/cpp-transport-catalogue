
#include "request_handler.h"

namespace request_handler {


RequestHandler::RequestHandler(TransportCatalog& tc): transport_catalog_(tc){
     CreateBusRoutesForMap();
}

void RequestHandler::CreateBusRoutesForMap(){
    std::deque<BusRoute> bus_routs = transport_catalog_.GetBusRouts();
    std::sort(bus_routs.begin(), bus_routs.end(), [](const BusRoute& a, const BusRoute& b){
        return a.bus_number<b.bus_number;
    });
    for(const BusRoute& bus_route : bus_routs){
        std::pair<std::string, std::vector<BusStopForMap>> br  {bus_route.bus_number, CreateBusRouteForMap(bus_route)};
        buses_routes_.push_back(br);
    }
}
std::vector<BusStopForMap> RequestHandler::CreateBusRouteForMap(const BusRoute& bus_route){
    std::vector<BusStopForMap> stops_coordinates;
    for(const BusStop* bus_stop: bus_route.route){
        stops_coordinates.push_back({bus_stop->stop_name, bus_stop->stop_cord, bus_route.is_roundtrip});
    }
return stops_coordinates;
}

BusRoutes& RequestHandler::GetBusesRoutes(){
    return buses_routes_;
}

}//end namespace request_handler

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */
