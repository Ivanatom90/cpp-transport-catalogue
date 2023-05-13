
#include "input_reader.h"



Request ReadInput(std::string& str){
    int pos = str.find_first_of(' ');
    Request req;
    req.type = str.substr(0, pos);
    str.erase(0, pos);
    pos = str.find_first_not_of(' ');
    str.erase(0, pos);
    req.request = str;
    return req;

}

int ReadOutputRequest(std::string& str){
    int pos = str.find_first_of(' ');
    str.erase(0, pos);
    pos = str.find_first_not_of(' ');
    str.erase(0, pos);
    pos = str.find(':');
    std::string s = str.substr(0, pos-1);
    int number_bus = stoi(s);
    return number_bus;

}

int RequestNumber(std::string& s){
    int first =  s.find_first_not_of(' ');
    int last = s.find_last_not_of(' ');
    s = s.substr(first, last-first +1);
    return stoi(s);
}

void RequestBase::GetRequestBase(std::istream& input){
    std::string s;
    getline(input, s);
    int request_count = RequestNumber(s); //количество запросов
    for (int i =0; i<request_count; i++){
        getline(input, s);
        Request req = ReadInput(s);
            if(req.type == "Stop"){
                requests_stops.push_back(req);
            } else{
                requests_bus.push_back(req);
            }
    }

}


void RequestBase::CreateTCatalog(){
    std::for_each(requests_stops.begin(), requests_stops.end(), [this](const Request& req){
        std::string str = req.request;
        std::string cord1, cord2, dis_s, next_stop;
        double dist;
        BusStop bs;
        StopDist sd;
        int pos = str.find(':');
        bs.stop_name = str.substr(0,pos);
        sd.stop1 = bs.stop_name;
        std::pair<std::string, std::string> stop1_stop2; // расстояние м/у остановками
        stop1_stop2.first = bs.stop_name;
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
               stop1_stop2.second = sd.stop2;
               TCatalog_.AddDistBetweenStops(stop1_stop2, dist);
            }
        TCatalog_.AddStop(bs);
    });

    for_each(requests_bus.begin(), requests_bus.end(), [this](const Request& req){
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
                    TCatalog_.GetBusStop(station)->buses_numbers.insert(number);
                    br.route.push_back(TCatalog_.GetBusStop(station));

                }
                if(trigger){
                    stops_dist.second = station;
                    br.distance += TCatalog_.GetDistaseBetweenStops(stops_dist);
                    cor_now = br.route.back()->stop_cord;
                    br.distance_geo += ComputeDistance(cor_last,  cor_now);
                }
                cor_last = br.route.back()->stop_cord;
                str.erase(0, pos);
                trigger = true;
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
                    TCatalog_.AddBusNumberToStop(station, number);
                    br.route.push_back(TCatalog_.GetBusStop(station));
                }
                if(trigger){
                    stops_dist.second = station;
                    br.distance += TCatalog_.GetDistaseBetweenStops(stops_dist);
                    cor_now = br.route.back()->stop_cord;
                    br.distance_geo += ComputeDistance(cor_last,  cor_now);
                }
                cor_last = br.route.back()->stop_cord;
                str.erase(0, pos);
                trigger = true;
                stops_dist.first = station;
            }

            br.distance_geo *= 2;
            int  u = br.route.size()-2;
            for (int i = u; i >= 0; i--){
                stops_dist.first = br.route.back()->stop_name;
                br.route.push_back(br.route[i]);
                stops_dist.second = br.route.back()->stop_name;                cor_now = br.route.back()->stop_cord;
                br.distance += TCatalog_.GetDistaseBetweenStops(stops_dist);

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
        TCatalog_.AddBusRoute(br);
    });
}

