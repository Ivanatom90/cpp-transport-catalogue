
#include "transport_catalogue.h"


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
    buses_routes_.push_back(br);
    routes_[br.bus_number] = &buses_routes_.back();
}
void TransportCatalog::AddDistBetweenStops(std::pair<std::string, std::string>& stop1_stop2, int dist){
    distanse_stop_[stop1_stop2] = dist;
    std::pair<std::string, std::string> s2_s1;
    s2_s1.second = stop1_stop2.first;
    s2_s1.first = stop1_stop2.second;
    if(!distanse_stop_.count(s2_s1)){
         distanse_stop_[s2_s1] = dist;
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

int TransportCatalog::GetDistaseBetweenStops(std::pair<std::string, std::string>& stops) const {
    return distanse_stop_.at(stops);
}

BusRoute* TransportCatalog::GetBusRoude(const std::string& bus) const{
    if (!routes_.count(bus)){
        BusRoute* br = nullptr;
        return br;}
    return routes_.at(bus);
}
