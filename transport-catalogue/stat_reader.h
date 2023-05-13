#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <ostream>
#include <sstream>
#include <istream>
#include <map>

#include "transport_catalogue.h"

struct Output{
      Output() = default;
      std::string bus_nember = "";
      int bus_stop_number_in_route = 0;
      int unique_bus_stop_number = 0;
      double route_lengh = 0;
      double curvature = 0;
      bool found = false;

};


class RequestOut{
    public:
        RequestOut(TransportCatalog, std::istream&);

        void GetOutPut();
//        void OutPutBus();
        void OutPutStop();
        void GetReqOut(std::istream&);
        void Out(Output&);


    private:
        TransportCatalog Catalog_;
        std::vector<std::string> bus_name_requests;
        Output output_;
        std::vector<Request> req_out_;

};




// напишите решение с нуля
// код сохраните в свой git-репозиторий
