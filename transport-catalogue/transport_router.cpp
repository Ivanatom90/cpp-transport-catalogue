#include "transport_router.h"

namespace transport_catalogue {
namespace detail {
namespace router {


TransportRouter::TransportRouter(transport_catalogue::TransportCatalog& tc):  graph_(tc.GetBusStops().size()*2), transport_catalog_(tc){
    routing_settings_ =  transport_catalog_.GetRouteSettings();
    CreateGraph();
}

void TransportRouter::CreateGraph(){
    //graph::DirectedWeightedGraph<double> grph(transport_catalog_.GetBusStops().size());
    size_t i = 0;
    for (const BusStop& stop:transport_catalog_.GetBusStops()){         
                graph::VertexId first = i++;
                graph::VertexId second = i++;
                stop_to_router_[stop.stop_name] = RouterByStop{first, second};
    }

    for (const auto [stop, num] : stop_to_router_) {
        graph::EdgeId id = graph_.AddEdge(graph::Edge<double>{num.bus_wait_start,
                                                 num.bus_wait_end,
                                                 routing_settings_.bus_wait_time});

        EdgeId_to_edge_[id] = {StopEdge{stop, routing_settings_.bus_wait_time}};

    }

    for(const BusRoute& bus_route:transport_catalog_.GetBusRouts()){
        MakeGraphFromRoute(bus_route);
    }
}

void TransportRouter::MakeGraphFromRoute(const BusRoute& bus_route){
   const std::vector<BusStop*>& stops =  bus_route.route;
   MakeGraphFromStops(stops.begin(), stops.end(), bus_route.bus_number);
   if(!bus_route.is_roundtrip){
        MakeGraphFromStops(stops.rbegin(), stops.rend(), bus_route.bus_number);
   }

}


const graph::DirectedWeightedGraph<double>& TransportRouter::GetGraph() const{
    return graph_;
}

RouterByStop TransportRouter::GetRouterByStop(StopName stopname) const{
    return stop_to_router_.at(stopname);
}

 std::variant<StopEdge, BusEdge> TransportRouter::GetEdge(graph::EdgeId id) const {return EdgeId_to_edge_.at(id);}

}//end namespace router
}//end namespace detail
}//end namespace transport_catalogue
