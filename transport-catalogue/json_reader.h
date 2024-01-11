#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "domain.h"
#include "map_renderer.h"
#include <vector>
#include <map>
#include <iostream>
#include <ostream>
#include "json_builder.h"
#include "transport_router.h"
#include "graph.h"

#include "log_duration.h"


namespace transport_catalogue {
namespace detail {
namespace json {

//using namespace transport_catalogue::detail::stop;
//using namespace transport_catalogue::detail::bus;

using  map_renderer::RenderSettings;

using Answer = std::map<std::string, std::variant<std::string, int, double, std::vector<std::string>>>;

using transport_catalogue::detail::router::TransportRouter;

struct StatRequest {
    int id;
    std::string type;
    std::string name;
    std::optional<std::string> from;
    std::optional<std::string> to;
};

struct Bus_J{
   std::string name;
   std::vector<std::string> route;
   bool is_roudtrip;

};

struct Stop_J{
  std::string name;
  double latitude;
  double longitude;
  std::map<std::string, int> dist_to;

};





class JSONr{
public:

    JSONr(Document& doc, std::ostream& ou);
    void BusStopAdd();
    void BusAdd();


    void ParseReq();
    void ParseBasReq(const Node& BasicRegnode);
    void ParseRenderSettings(const Node& BasicRegnode);
    void ParseStatReq(const Node& BasicRegnode);
    void ParseRoutingSettings(const Node& BasicRegnode);
    void CreateBusJ(const Node& bus);
    void CreateStopJ(const Node& stop);

    void CreateTransportCataloge();

    void CreateAnswer();
    Array CreateAnsRoute(Dict& dict);
    void PrintAnswer();

    void SetMap(std::string mapa){
        mapa_ = mapa;
    }

    void SetTransportRouter(std::unique_ptr<TransportRouter> tr);

    TransportCatalog& GetTransportCataloge();
    RenderSettings& GetRenderSettings();




private:
    json::Document& document_;
    TransportCatalog transport_catalogue_;
    std::unique_ptr<TransportRouter> transport_router_;
    std::unique_ptr<graph::Router<double>> router_ptr_;
    std::vector<Bus_J> buses_j_;
    std::vector<Stop_J> stops_j_;
    std::vector<StatRequest> stat_reqs_;
    std::vector<Answer> answers_;
    std::string mapa_;
    RenderSettings render_settings_;

    RoutingSettings route_settings_;

    std::ostream& out;


    svg::Color DetermineColor(const Node& color);
};




}//end namespace json
}//end namespace detail
}//end namespace transport_catalogue
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
