
#include "stat_reader.h"
#include "input_reader.h"
#include <set>

    RequestOut::RequestOut(TransportCatalog TC, std::istream& in): Catalog_(TC){
        //bus_name_requests = GetReqOut(in);
        GetReqOut(in);
        GetOutPut();
    }

    void RequestOut::GetOutPut(){
        for(Request& r:req_out_){
             if(r.type == "Bus"){
                    Output ou;
                    BusRoute* br = Catalog_.FindBus(r.request);
                    bool not_empty = br->not_empty;
                    if(!not_empty){
                        ou.bus_nember =  r.request;
                        ou.found =false;
                        //output_.push_back(ou);
                        delete br;
                        std::cout<<"Bus "<<ou.bus_nember<<": not found"<<std::endl;
                    } else{
                        ou.bus_nember = r.request;
                        ou.bus_stop_number_in_route = Catalog_.FindBus(r.request)->route_number;;
                        ou.route_lenght =  Catalog_.FindBus(r.request)->distanse;
                        ou.unico_bus_stop_number = Catalog_.FindBus(r.request)->unic_route_number;
                        ou.izvil = Catalog_.FindBus(r.request)->izvil;
                        ou.found =true;
                        //output_.push_back(ou);
                        br = nullptr;
                        std::cout<<"Bus "<<ou.bus_nember<<": "<<ou.bus_stop_number_in_route<<" stops on route, "
                                <<ou.unico_bus_stop_number<<" unique stops, "<<ou.route_lenght
                               <<" route length, "<<ou.izvil<<" curvature"<<std::endl;
                    }


            } else {
                 BusStop* bs = Catalog_.FindStop(r.request);
                 bool not_empty = bs->not_empty;
                 if(not_empty && (bs->buses_numbers.size())){
                     std::cout<<"Stop "<<r.request<<": buses";
                     for (const std::string num:bs->buses_numbers){
                         std::cout<<" "<<num;
                     }
                     std::cout<<std::endl;
                     bs = nullptr;
                 } else if(not_empty){
                           std::cout<<"Stop "<<r.request<<": no buses"<<std::endl;
                           bs = nullptr;
                        }
                    else
                 {
                     std::cout<<"Stop "<<r.request<<": not found"<<std::endl;
                     delete bs;
                 }
             }
        }



    }

void RequestOut::OutPutBus(){
    for(Output op:output_){
        (op.found)
           ? std::cout<<"Bus "<<op.bus_nember<<": "<<op.bus_stop_number_in_route<<" stops on route, "<<op.unico_bus_stop_number<<" unique stops, "<<op.route_lenght<<" route length"<<std::endl
           : std::cout<<"Bus "<<op.bus_nember<<": not found"<<std::endl;
    }
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
