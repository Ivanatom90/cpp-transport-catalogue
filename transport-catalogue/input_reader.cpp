
#include "input_reader.h"


//RequestBase::RequestBase(std::pair<std::vector<Request>, std::vector<Request>> p): requests_stop(p.first), requests_bus(p.second){}

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

RequestBase GetRequestBase(std::istream& input){
    std::string s;
    RequestBase base;
    getline(input, s);
    int request_count = RequestNumber(s); //количество запросов
    for (int i =0; i<request_count; i++){
        getline(input, s);
        Request req = ReadInput(s);
            if(req.type == "Stop"){
                base.requests_stops.push_back(req);
            } else{
                base.bus_stops.push_back(req);
            }
    }


    return base;
}


std::vector<Request>* RequestBase::GetStops(){
    return &requests_stops;
}
std::vector<Request>* RequestBase::GetBusStops(){
    return &bus_stops;
}

// напишите решение с нуля
// код сохраните в свой git-репозиторий
