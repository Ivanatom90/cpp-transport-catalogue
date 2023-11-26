
#include "transport_catalogue.h"

using namespace transport_catalogue;

size_t TransportCatalog::Hash::operator()(const std::pair<std::string, std::string>& s) const{
    size_t s1, s2;
    s1 = s.first.size();
    s2 = s.second.size();
    return static_cast<size_t>(s1*3 + s2*37);
}

void TransportCatalog::AddStop(BusStop& bs){
    stops_base_.push_back(bs);
    stops_[std::string(stops_base_.back().stop_name)] = &stops_base_.back();
}
void TransportCatalog::AddBusRoute(BusRoute& br){
    std::pair<std::string, std::string> s1_s2;
    std::string stop1, stop2;

    if(!br.is_roundtrip){

        for(size_t i = 0; i<br.route.size()-1; i++){
            stop1 = br.route[i]->stop_name;
            stop2 = br.route[i+1]->stop_name;
            br.distance += GetDistaseBetweenStops(stop1, stop2);
            br.distance_geo += transport_catalogue::detail::geo::Compute_distance(br.route[i]->stop_cord, br.route[i+1]->stop_cord);
        }

        for(size_t i = br.route.size()-1; i>0; i--){
            stop1 = br.route[i]->stop_name;
            stop2 = br.route[i-1]->stop_name;
            br.distance += GetDistaseBetweenStops(stop1, stop2);
            br.distance_geo += transport_catalogue::detail::geo::Compute_distance(br.route[i]->stop_cord, br.route[i-1]->stop_cord);
        }
        br.curvature = double(br.distance/br.distance_geo);

    } else {
        for(size_t i = 0; i<br.route.size()-1; i++){
            stop1 = br.route[i]->stop_name;
            stop2 = br.route[i+1]->stop_name;
            br.distance += GetDistaseBetweenStops(stop1, stop2);
            br.distance_geo += transport_catalogue::detail::geo::Compute_distance(br.route[i]->stop_cord, br.route[i+1]->stop_cord);
        }
        br.curvature = double(br.distance/br.distance_geo);
    }
    std::set<BusStop*> unic_stops;
    for(BusStop* busstop: br.route){
        unic_stops.insert(busstop);
        AddBusNumberToStop(busstop->stop_name, br.bus_number);
    }
    br.unique_route_number = unic_stops.size();
    buses_routes_.push_back(br);
    routes_[br.bus_number] = &buses_routes_.back();

}

void TransportCatalog::AddDistBetweenStops(const std::string& stop1, const std::string& stop2, int dist){
    std::pair<std::string, std::string> stops(stop1, stop2);
    distanse_stop_[stops] = dist;
    stops  = {stop2, stop1};
    if(!distanse_stop_.count(stops)){
         distanse_stop_[stops] = dist;
    }
}

void TransportCatalog::AddBusNumberToStop(std::string& stop_name, std::string& bus_number){
    stops_[stop_name]->buses_numbers.insert(bus_number);
}

BusStop* TransportCatalog::GetBusStop(const std::string& stop) const{
    if (!stops_.count(stop)){
        BusStop* bs = nullptr;
        return bs;
    }
    return stops_.at(stop);
}

int TransportCatalog::GetDistaseBetweenStops(std::string& stop1, std::string& stop2) const {
    std::pair<std::string, std::string> stops(stop1, stop2);
    return distanse_stop_.at(stops);
}

BusRoute* TransportCatalog::GetBusRoude(const std::string& bus) const{
    if (!routes_.count(bus)){
        BusRoute* br = nullptr;
        return br;}
    return routes_.at(bus);
}


const std::deque<BusStop>& TransportCatalog::GetBusStops() const{
    return stops_base_;
}

bool TransportCatalog::BusStopExist(const std::string& stop) const{
    return count_if(stops_base_.begin(), stops_base_.end(), [&stop](const BusStop& bs){
        return bs.stop_name == stop;
    });
}
bool TransportCatalog::BusRoudeExist(const std::string& bus) const{
    return count_if(buses_routes_.begin(), buses_routes_.end(), [&bus](const BusRoute& br){
        return br.bus_number == bus;
    });
}

const std::deque<BusRoute>& TransportCatalog::GetBusRouts() const{
    return buses_routes_;
}
