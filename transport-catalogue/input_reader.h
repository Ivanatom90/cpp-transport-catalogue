#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <ostream>
#include <sstream>
#include <istream>
#include <map>

struct  Request
{
    std::string type;
    std::string request;

};

class RequestBase{
public:
//    RequestBase(std::pair<std::vector<Request>, std::vector<Request>>);
    std::vector<Request> requests_stops; //точки остановки
    std::vector<Request> bus_stops; //маршруты автобусов

    std::vector<Request>* GetStops();
    std::vector<Request>* GetBusStops();
    private:
};

Request ReadInput(std::string& str);
int RequestNumber(std::string& str);
RequestBase GetRequestBase(std::istream& input);
