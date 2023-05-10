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
      int unico_bus_stop_number = 0;
      double route_lenght = 0;
      double izvil = 0;
      bool found = false;

};


class RequestOut{
    public:
        RequestOut(TransportCatalog, std::istream&);

        void GetOutPut();
        void OutPutBus();
        void OutPutStop();
        void GetReqOut(std::istream&);


    private:
        TransportCatalog Catalog_;
        std::vector<std::string> bus_name_requests;
        std::vector<Output> output_;
        //std::vector<OutputBus> output_bus_in_stop;
        std::vector<Request> req_out_;

};




// напишите решение с нуля
// код сохраните в свой git-репозиторий
