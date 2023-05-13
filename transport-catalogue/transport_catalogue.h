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

#include "geo.h"



struct StopDist{
  StopDist() = default;
  std::string stop1 = "";
  std::string stop2 = "";
  int dist = 0;
};




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
      double distance =0;
      double distance_geo = 0;
      double curvature = 0;
      int route_number =0;
      int unique_route_number =0;
      bool not_empty = false;
};


class TransportCatalog{
    public:

    void AddStop(BusStop& bs);
    void AddBusRoute(BusRoute& br);
    void AddDistBetweenStops(std::pair<std::string, std::string>& stop1_stop2, int dist);
    void AddBusNumberToStop(std::string& stop_name, std::string& bus_number);

    BusStop* GetBusStop(const std::string& stop) const;
    BusRoute* GetBusRoude(const std::string& bus) const;
    int GetDistaseBetweenStops(std::pair<std::string, std::string>& stops) const;

        class Hash{

          public:
            size_t operator()(const std::pair<std::string, std::string>&) const;
        };

private:
    std::deque<BusStop> stops_base_;
    std::deque<BusRoute> buses_routes_;
    std::unordered_map<std::pair<std::string, std::string>, int, Hash> distanse_stop_;
    std::unordered_map<std::string, BusStop*> stops_;
    std::unordered_map<std::string, BusRoute*> routes_;
};

