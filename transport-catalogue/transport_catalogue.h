#pragma once
#include <deque>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <string_view>
#include <algorithm>
#include <set>
#include <unordered_set>

#include "input_reader.h"
#include "geo.h"

struct BusStop{
    BusStop() = default;
    std::string stop_name;
    Coordinates stop_cord;
    std::set<std::string> buses_numbers;
    bool not_empty = false;
};

struct BusRoute{
      BusRoute() = default;
      std::string bus_number = "";
      bool type_circle = false;
      std::vector<BusStop*> route;
      double distanse =0;
      double distanse_geo = 0;
      double izvil = 0;
      int route_number =0;
      int unic_route_number =0;
      bool not_empty = false;
};

struct StopDist{
      StopDist() = default;
      std::string stop1 = "";
      std::string stop2 = "";
      int dist = 0;
};



class TransportCatalog{
    public:
        TransportCatalog() = default;
        TransportCatalog(RequestBase rb):requvests_(rb){
            AddRb(rb);
        }

        BusStop* FindStop(std::string);
        BusRoute* FindBus  (std::string) const;

        void GetBusInfo(int);

        class Hash{

          public:
            size_t operator()(const std::pair<std::string, std::string>&) const;
        };
    private:
        void AddStop(std::string, Coordinates);
        void AddRb(RequestBase&);
        void AddBusRoute();




    RequestBase requvests_;
    std::deque<BusStop> stops_base;
    std::deque<BusRoute> buses_routes_;
    std::unordered_map<std::string, BusRoute*> routes_;
    std::unordered_map<std::string, BusStop*> stops_;
    std::unordered_map<std::pair<std::string, std::string>, int, Hash> distanse_stop_;
};
// напишите решение с нуля
// код сохраните в свой git-репозиторий

