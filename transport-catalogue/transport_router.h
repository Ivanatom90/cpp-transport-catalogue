#pragma once

#include "transport_catalogue.h"
#include "router.h"
#include "domain.h"
#include "graph.h"

#include <deque>
#include <unordered_map>
#include <iostream>
#include <variant>
#include <memory>
#include <algorithm>

namespace transport_catalogue {
namespace detail {
namespace router {

//using namespace domain;
//using namespace graph;

using StopName = std::string;

struct RouterByStop {
    graph::VertexId bus_wait_start;
    graph::VertexId bus_wait_end;
};

struct StopEdge {
    std::string name;
    double time = 0;
};

struct BusEdge {
    std::string bus_name;
    size_t span_count = 0;
    double time = 0;
};

struct RouteInfo {
    double total_time = 0.;
    std::vector<std::variant<StopEdge, BusEdge>> edges;
};

class TransportRouter {
public:
    TransportRouter(TransportCatalog&);

    void CreateGraph();
    void MakeGraphFromRoute(const BusRoute& bus_route);

    template<typename Iterator>
    void MakeGraphFromStops(Iterator start, Iterator fin, std::string busname);
    void MakeRouter();
    std::variant<StopEdge, BusEdge> GetEdge(graph::EdgeId id) const;
    void SetStops();




    RouterByStop GetRouterByStop(StopName stopname) const;


    const graph::DirectedWeightedGraph<double>& GetGraph() const;


private:

    graph::DirectedWeightedGraph<double> graph_;
    TransportCatalog& transport_catalog_;
    RoutingSettings routing_settings_;
    std::unordered_map<StopName, RouterByStop> stop_to_router_;
    std::unordered_map<graph::EdgeId, std::variant<StopEdge, BusEdge>> EdgeId_to_edge_;
};


template<typename Iterator>
void TransportRouter::MakeGraphFromStops(Iterator start, Iterator fin, std::string busname){

    for(auto it = start; it != fin; it++){
        size_t distance  = 0;
        size_t span = 0;
        for(auto it2 = next(it); it2 != fin; it2++){
            graph::Edge<double> edge;
            edge.from = stop_to_router_.at((*it)->stop_name).bus_wait_end;
            edge.to = stop_to_router_.at((*it2)->stop_name).bus_wait_start;
            distance += transport_catalog_.GetDistaseBetweenStops((*prev(it2))->stop_name, (*it2)->stop_name);
            edge.weight = double(distance/((routing_settings_.bus_velocity)*1000/60));
            ++span;
            graph::EdgeId id = graph_.AddEdge(edge);
            EdgeId_to_edge_[id] = BusEdge{busname, span, graph_.GetEdge(id).weight};
        }
    }
}


}//end namespace router
}//end namespace detail
}//end namespace transport_catalogue
