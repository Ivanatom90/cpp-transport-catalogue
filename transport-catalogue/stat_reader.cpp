
#include "stat_reader.h"
#include "input_reader.h"
#include <ostream>
#include <set>

    RequestOut::RequestOut(TransportCatalog& TC, std::istream& in): Catalog_(TC){
        GetReqOut(in);

    }

    void RequestOut::GetOutPutBus(const Request& req, std::ostream& out){
        Output output;
        BusRoute* bus_route = Catalog_.GetBusRoude(req.request);
        if(bus_route == nullptr){
            output.bus_nember =  req.request;
            output.found =false;
            bus_route  = nullptr;
            out<<"Bus "<<output.bus_nember<<": not found"<<std::endl;
        } else{
            output.bus_nember = req.request;
            output.bus_stop_number_in_route = bus_route->route_number;
            output.route_lengh =  bus_route->distance;
            output.unique_bus_stop_number = bus_route->unique_route_number;
            output.curvature = bus_route->curvature;
            output.found =true;
            bus_route = nullptr;
            Out(output);
        }
    }
    void RequestOut::GetOutPutStop(const Request& req, std::ostream& out){
        BusStop* bus_stop = Catalog_.GetBusStop(req.request);
        if(bus_stop != nullptr && (bus_stop->buses_numbers.size())){
            out<<"Stop "<<req.request<<": buses";
            for (const std::string num:bus_stop->buses_numbers){
               out<<" "<<num;
            }
            out<<std::endl;
            bus_stop = nullptr;
        } else if(bus_stop != nullptr){
                  out<<"Stop "<<req.request<<": no buses"<<std::endl;
                  bus_stop = nullptr;
               }
           else
        {
            out<<"Stop "<<req.request<<": not found"<<std::endl;
             bus_stop = nullptr;
        }
    }

    void RequestOut::GetOutPut(){
        for(const Request& req:req_out_){
             if(req.type == "Bus"){
                    GetOutPutBus(req, std::cout);
                } else {
                    GetOutPutStop(req, std::cout);
             }
        }
    }

    void RequestOut::Out(Output& output){
        std::cout<<"Bus "<<output.bus_nember<<": "<<output.bus_stop_number_in_route<<" stops on route, "
                <<output.unique_bus_stop_number<<" unique stops, "<<output.route_lengh
               <<" route length, "<<output.curvature<<" curvature"<<std::endl;
    }


void RequestOut::GetReqOut(std::istream& input){
    std::string s, word;
    std::vector<std::string> result;
    getline(input, s);
    int pos;
    int request_count = RequestNumber(s); //количество запросов
    Request rt;
    for (int i = 0; i<request_count; i++){
        getline(input, s);
        pos = s.find(' ');
        word = s.substr(0,pos);
        rt.type = word;
        s.erase(0, pos);
        pos = s.find_first_not_of(' ');
        s.erase(0, pos);
        pos = s.find_last_not_of(" \r");
        word = s.substr(0, pos+1);
        rt.request = word;
        req_out_.push_back(rt);
    }

}
