
#include "stat_reader.h"
#include "input_reader.h"
#include <ostream>
#include <set>

    RequestOut::RequestOut(TransportCatalog TC, std::istream& in): Catalog_(TC){
        GetReqOut(in);
        GetOutPut();
    }

    void RequestOut::GetOutPut(){
        for(Request& req:req_out_){
             if(req.type == "Bus"){
                    Output output;
                    BusRoute* br = Catalog_.FindBus(req.request);
                    bool not_empty = br->not_empty;
                    if(!not_empty){
                        output.bus_nember =  req.request;
                        output.found =false;
                        delete br;
                         std::cout<<"Bus "<<output.bus_nember<<": not found"<<std::endl;
                    } else{
                        output.bus_nember = req.request;
                        output.bus_stop_number_in_route = Catalog_.FindBus(req.request)->route_number;;
                        output.route_lengh =  Catalog_.FindBus(req.request)->distance;
                        output.unique_bus_stop_number = Catalog_.FindBus(req.request)->unique_route_number;
                        output.curvature = Catalog_.FindBus(req.request)->curvature;
                        output.found =true;
                        br = nullptr;
                        Out(output);
                    }


            } else {
                 BusStop* bs = Catalog_.FindStop(req.request);
                 bool not_empty = bs->not_empty;
                 if(not_empty && (bs->buses_numbers.size())){
                     std::cout<<"Stop "<<req.request<<": buses";
                     for (const std::string num:bs->buses_numbers){
                         std::cout<<" "<<num;
                     }
                     std::cout<<std::endl;
                     bs = nullptr;
                 } else if(not_empty){
                           std::cout<<"Stop "<<req.request<<": no buses"<<std::endl;
                           bs = nullptr;
                        }
                    else
                 {
                     std::cout<<"Stop "<<req.request<<": not found"<<std::endl;
                     delete bs;
                 }
             }
        }

    }

    void RequestOut::Out(Output& output){
        std::cout<<"Bus "<<output.bus_nember<<": "<<output.bus_stop_number_in_route<<" stops on route, "
                <<output.unique_bus_stop_number<<" unique stops, "<<output.route_lengh
               <<" route length, "<<output.curvature<<" curvature"<<std::endl;
    }

/*
void RequestOut::OutPutBus(){
    for(Output op:output_){
        (op.found)
           ? std::cout<<"Bus "<<op.bus_nember<<": "<<op.bus_stop_number_in_route<<" stops on route, "<<op.unique_bus_stop_number
                     <<" unique stops, "<<op.route_lengh<<" route length"<<std::endl
           : std::cout<<"Bus "<<op.bus_nember<<": not found"<<std::endl;
    }
}
*/
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

// напишите решение с нуля
// код сохраните в свой git-репозиторий
