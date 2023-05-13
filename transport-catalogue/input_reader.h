#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <ostream>
#include <sstream>
#include <istream>
#include <map>
#include <set>

#include "transport_catalogue.h"
#include "geo.h"


struct  Request
{
    std::string type;
    std::string request;

};

class RequestBase{
public:

    RequestBase(std::istream& input){
        GetRequestBase(input);
        CreateTCatalog();
    }


    TransportCatalog TCatalog_;
    private:

    std::vector<Request> requests_stops; //точки остановки
    std::vector<Request> requests_bus; //маршруты автобусов
    void CreateTCatalog();
    void GetRequestBase(std::istream& input);
};

Request ReadInput(std::string& str);
int RequestNumber(std::string& str);

