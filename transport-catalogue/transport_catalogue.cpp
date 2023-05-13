
#include "transport_catalogue.h"


size_t TransportCatalog::Hash::operator()(const std::pair<std::string, std::string>& s) const{
    size_t s1, s2;
    s1 = s.first.size();
    s2 = s.second.size();
    return static_cast<size_t>(s1*3 + s2*37);
}

void TransportCatalog::AddStop(const std::string& stop_name, Coordinates cord){
    BusStop bs;
    bs.stop_name = stop_name;
    bs.stop_cord = cord;
    stops_base.push_back(bs);
    stops_[std::string(stops_base.back().stop_name)] = &stops_base.back();
}


void TransportCatalog::AddRb(RequestBase& rb){
    std::for_each(rb.requests_stops.begin(), rb.requests_stops.end(), [this](const Request& req){
        std::string str = req.request;
        std::string cord1, cord2, dis_s, next_stop;

        double dist;
        BusStop bs;
        StopDist sd;
        int pos = str.find(':');
        bs.stop_name = str.substr(0,pos);
        sd.stop1 = bs.stop_name;
        std::pair<std::string, std::string> st; // расстояние м/у остановками
        st.first = bs.stop_name;
        str.erase(0, pos);
        pos = str.find_first_not_of(": ");
        str.erase(0,pos);
        pos = str.find(',');
        cord1 = str.substr(0, pos);
        bs.stop_cord.lat = stod(cord1);
        str.erase(0,pos);
        pos = str.find_first_not_of(", ");
        str.erase(0, pos);
        pos = str.find(',');
        cord2 = str.substr(0,pos);
        str.erase(0, pos);
        bs.stop_cord.lng = stod(cord2);
        bs.not_empty = true;
            while(pos != str.npos){
               pos = str.find_first_not_of(", ");
               str.erase(0, pos);
               pos = str.find('m');
               dis_s = str.substr(0, pos);
               dist = stoi(dis_s);
               str.erase(0, pos+1);
               pos = str.find_first_not_of(' ');
               pos = str.find_first_not_of(' ', pos+3);
               str.erase(0, pos);
               pos = str.find(',');
               next_stop = str.substr(0, pos);
               str.erase(0, pos+1);
               sd.stop2 = next_stop;
               sd.dist = dist;
               st.second = sd.stop2;
               distanse_stop_[st] = dist;
               st.first = sd.stop2;
               st.second = sd.stop1;
               if(!distanse_stop_.count(st)){
                    distanse_stop_[st] = dist;
               }
               st.first = sd.stop1;
            }
        stops_base.push_back(bs);
        stops_[std::string(stops_base.back().stop_name)] = &stops_base.back();
    });

    for_each(rb.bus_stops.begin(), rb.bus_stops.end(), [this](const Request& req){
        std::string str = req.request;
        int pos = str.find_first_not_of(": ");
        str.erase(0,pos);
        pos = str.find(':');
        BusRoute br;
        std::string number = str.substr(0, pos);
        pos = number.find_last_not_of(" :");
        number.substr(0, pos+1);
        br.bus_number = number;
        str.erase(0, pos+1);
        pos = str.find_first_not_of(": ");
        str.erase(0, pos);
        std::string station;
        int pos2;
        bool trigger = false;
        Coordinates cor_last, cor_now;
        StopDist sd;
        std::pair<std::string, std::string> stops_dist;
        if(str.find('>') != str.npos){
            br.type_circle = false;
            while(pos != str.npos){

                pos = str.find_first_not_of("> ");
                str.erase(0, pos);
                pos = str.find('>');
                station = str.substr(0, pos);
                pos2= station.find_last_not_of(' ');
                station = station.substr(0, pos2+1);

                if (station != ">" && station != "-"){
                    stops_[station]->buses_numbers.insert(number);
                    br.route.push_back(stops_.at(station));

                }
                if(trigger){
                    //sd.stop2 = station;
                    stops_dist.second = station;
                    br.distance += distanse_stop_[stops_dist];
                    cor_now = br.route.back()->stop_cord;
                    br.distance_geo += ComputeDistance(cor_last,  cor_now);
                }
                cor_last = br.route.back()->stop_cord;
                str.erase(0, pos);
                trigger = true;
                //sd.stop1 = station;
                stops_dist.first = station;
            }

        } else {
            br.type_circle = true;
            StopDist sd;
            while(pos != str.npos){
                pos = str.find_first_not_of("- ");
                str.erase(0, pos);
                pos = str.find('-');
                station = str.substr(0, pos);
                pos2= station.find_last_not_of(' ');
                station = station.substr(0, pos2+1);
                if (station != ">" && station != "-") {
                    stops_[station]->buses_numbers.insert(number);
                    br.route.push_back(stops_.at(station));
                }
                if(trigger){
                   // sd.stop2 = station;
                    stops_dist.second = station;
                    br.distance += distanse_stop_[stops_dist];
                    cor_now = br.route.back()->stop_cord;
                    br.distance_geo += ComputeDistance(cor_last,  cor_now);
                }
                cor_last = br.route.back()->stop_cord;
                str.erase(0, pos);
                trigger = true;
                //sd.stop1 = station;
                stops_dist.first = station;
            }

            br.distance_geo *= 2;
            int  u = br.route.size()-2;
            for (int i = u; i >= 0; i--){
                stops_dist.first = br.route.back()->stop_name;
              //  cor_last = br.route.back()->stop_cord;
                br.route.push_back(br.route[i]);
                stops_dist.second = br.route.back()->stop_name;                cor_now = br.route.back()->stop_cord;
                br.distance += distanse_stop_[stops_dist];
              //  cor_now = br.route.back()->stop_cord;
               // br.distanse_geo += ComputeDistance(cor_last,  cor_now);
            }


        }
        br.route_number = br.route.size();
        auto& mark = br.route;
        std::set<std::string_view> unic;
        for (BusStop* stop:mark){
            unic.insert(stop->stop_name);
        }
        br.unique_route_number = unic.size();
        br.not_empty = true;
        br.curvature = br.distance/br.distance_geo;
        buses_routes_.push_back(br);
        routes_[br.bus_number] = &buses_routes_.back();


    });
}

void TransportCatalog::AddBusRoute(){

}

BusStop* TransportCatalog::FindStop(const std::string& bus_stop){
    if (!stops_.count(bus_stop)){
        BusStop* bs = new BusStop;
        bs->not_empty = false;
        return bs;
    }
    return stops_[bus_stop];
}
BusRoute* TransportCatalog::FindBus(const std::string& number) const {
    if (!routes_.count(number)){
        BusRoute* br = new BusRoute;
        br->not_empty = false;
        return br;}
    return routes_.at(number);
}
void TransportCatalog::GetBusInfo(int){

}
