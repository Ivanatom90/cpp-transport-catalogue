#include "json_reader.h"

namespace transport_catalogue {
namespace detail {
namespace json {

JSONr::JSONr(Document& doc, std::ostream& ou): document_(doc), out(ou) {}

void JSONr::ParseReq(){
    Dict main_node;
    if (document_.GetRoot().IsDict()){
        main_node = document_.GetRoot().AsDict();
            JSONr::ParseBasReq(main_node.at("base_requests"));
            JSONr::ParseRenderSettings(main_node.at("render_settings"));
            JSONr::ParseStatReq(main_node.at("stat_requests"));
            JSONr::ParseRoutingSettings(main_node.at("routing_settings"));
    }
}

void JSONr::ParseRoutingSettings(const Node& BasicRegNode){
    if(!BasicRegNode.IsDict()){
       return;
    }
    const Dict& node_route_set = BasicRegNode.AsDict();
    route_settings_ = {node_route_set.at("bus_wait_time").AsDouble(), node_route_set.at("bus_velocity").AsDouble()};
}


void JSONr::ParseBasReq(const Node& BasicRegNode){

    std::vector<Node> buses_nodes;
    std::vector<Node> stops_nodes;

    if (BasicRegNode.IsArray()){
            const Array& node_base = BasicRegNode.AsArray();
            for(const Node& node:node_base){
                if(node.IsDict()){
                    const Dict&  reg_node = node.AsDict();
                    if(reg_node.at("type").AsString() == "Bus"){
                        buses_nodes.push_back(node);
                    } else if(reg_node.at("type").AsString() == "Stop"){
                        stops_nodes.push_back(node);
                    } else {
                        throw "parse node don't have value";
                    }
                }
            }
    }

    for(const Node& bus:buses_nodes){
        CreateBusJ(bus);
    }

    for(const Node& stop:stops_nodes){
        CreateStopJ(stop);
    }

}


void JSONr::CreateBusJ(const Node& bus){
    Bus_J bus_js;
    Dict bus_node;

    if(bus.IsDict()){
        bus_node = bus.AsDict();
        bus_js.name = bus_node.at("name").AsString();
        for(const Node& n:bus_node.at("stops").AsArray()){
            bus_js.route.push_back(n.AsString());
        }
        bus_js.is_roudtrip = bus_node.at("is_roundtrip").AsBool();
    } else {
        throw "not create BusJ";
    }
    buses_j_.push_back(bus_js);

}

void JSONr::CreateStopJ(const Node& stop){
    Stop_J stop_js;
    Dict stop_node;
    if(stop.IsDict()){
        stop_node = stop.AsDict();
        stop_js.name = stop_node.at("name").AsString();
        stop_js.latitude = stop_node.at("latitude").AsDouble();
        stop_js.longitude = stop_node.at("longitude").AsDouble();
        for(const auto& dist:stop_node.at("road_distances").AsDict()  ){

                std::string key = dist.first;
                stop_js.dist_to[dist.first] = dist.second.AsInt();

        }
    stops_j_.push_back(stop_js);
    } else {
        throw "not stop reqvest";
    }
}

void JSONr::ParseStatReq(const Node& BasicRegnode){
    StatRequest sr;
    if(BasicRegnode.IsArray()){
       for(const Node& node:BasicRegnode.AsArray()){
           try{
               const Dict& dict = node.AsDict();

               if(dict.at("type").AsString() == "Route"){
                                  sr.id = dict.at("id").AsInt();
                                  sr.from = dict.at("from").AsString();
                                  sr.to = dict.at("to").AsString();
                                  sr.type = dict.at("type").AsString();
               } else if(dict.at("type").AsString() != "Map"){
                   sr.id = dict.at("id").AsInt();
                   sr.name = dict.at("name").AsString();
                   sr.type = dict.at("type").AsString();
               } else {
                   sr.id = dict.at("id").AsInt();
                   sr.type = dict.at("type").AsString();
               }

               stat_reqs_.push_back(sr);
           } catch(...){

           }
        }
    } else {
       throw "ParseStatReq error";
    }

}


svg::Color JSONr::DetermineColor(const Node& color){
    if(color.IsArray()){
        if(color.AsArray().size() == 4){
            const Array& rgba = color.AsArray();
            return svg::Rgba(rgba[0].AsInt(), rgba[1].AsInt(),rgba[2].AsInt(), rgba[3].AsDouble());
        } else if(color.AsArray().size() == 3){
            const Array& rgb = color.AsArray();
            return svg::Rgb(rgb[0].AsInt(), rgb[1].AsInt(),rgb[2].AsInt());
        }
    }
        return color.AsString();

}

void JSONr::ParseRenderSettings(const Node& BasicRegnode){
    RenderSettings& rs = render_settings_;
    if(BasicRegnode.IsDict()){
            const Dict& dict = BasicRegnode.AsDict();
            rs.width = dict.at("width").AsDouble();
            rs.height = dict.at("height").AsDouble();
            rs.padding = dict.at("padding").AsDouble();
            rs.stop_radius = dict.at("stop_radius").AsDouble();
            rs.line_width = dict.at("line_width").AsDouble();
            rs.bus_label_font_size = dict.at("bus_label_font_size").AsInt();
            rs.bus_label_offset = {dict.at("bus_label_offset").AsArray()[0].AsDouble(),dict.at("bus_label_offset").AsArray()[1].AsDouble()} ;
            rs.stop_label_offset = {dict.at("stop_label_offset").AsArray()[0].AsDouble(),dict.at("stop_label_offset").AsArray()[1].AsDouble()};
            rs.underlayer_width = dict.at("underlayer_width").AsDouble();
            rs.underlayer_color = DetermineColor(dict.at("underlayer_color"));
            rs.stop_label_font_size = dict.at("stop_label_font_size").AsInt();
            const Array& palette = dict.at("color_palette").AsArray();
            for(const Node& p : palette){
                rs.color_palette.push_back(DetermineColor(p));
            }

    }
}

void JSONr::CreateTransportCataloge(){
    for(const Stop_J& stop : stops_j_){
        BusStop bs;
        bs.stop_name = stop.name;
        bs.stop_cord = geo::Coordinates {stop.latitude, stop.longitude};
        transport_catalogue_.AddStop(bs);
        for(const std::pair<std::string, int> dist : stop.dist_to){
            transport_catalogue_.AddDistBetweenStops(stop.name, dist.first, dist.second);
        }
    }

    for(const Bus_J& bus: buses_j_){
        BusRoute br;
        br.bus_number = bus.name;
        br.is_roundtrip = bus.is_roudtrip;
        for(const std::string& stop :bus.route){
            br.route.push_back(transport_catalogue_.GetBusStop(stop));

        }
        transport_catalogue_.AddBusRoute(br);

    }
    transport_catalogue_.AddRouteSettings(route_settings_);

}



void JSONr::CreateAnswer(){
    //LogDuration ldCA("Total time answer create ");
    Builder answer_build;
    Array array_data;
    //answer_build.StartArray();
    for(const StatRequest& sr:stat_reqs_){
        //Answer answer;

        Dict data;
        if(sr.type == "Bus"){
           // LogDuration cre_ans_bus("CreateAnswerBus");
             if(transport_catalogue_.BusRoudeExist(sr.name)){
                const BusRoute* br =transport_catalogue_.GetBusRoude(sr.name);


                data["curvature"] = br->curvature;
                data["request_id"] =sr.id;
                data["route_length"] = int(br->distance);
                data["unique_stop_count"] = br->unique_route_number;

                (br->is_roundtrip)
                        ? data["stop_count"]=int(br->route.size())
                        : data["stop_count"]= ((int(br->route.size())*2)-1);

                      //   answer_build.Value(data);

             } else {
                         data["request_id"] = sr.id;
                         data["error_message"] = "not found";

                        // answer_build.Value(data);
                }

        } else if(sr.type == "Stop"){
               // LogDuration cre_ans_stop("CreateAnswerStop");

                data["request_id"] = sr.id;
                if(transport_catalogue_.BusStopExist(sr.name)){
                Array buses;
                const BusStop* bs = transport_catalogue_.GetBusStop(sr.name);

                for(const std::string& bus_number:bs->buses_numbers){
                    buses.push_back(bus_number);

                }
                data["buses"] = buses;
               // answer_build.Value(data);
            } else {
                  data["error_message"] = "not found";
                 // answer_build.Value(data);
                //answer["error_message"] = "not found";
            }
        } else if(sr.type == "Map"){
                      data["request_id"] = sr.id;
                      std::string str = "";
                for(const char ch:mapa_){
                    if(ch == '"' || ch == '\\'){
                        str += '\\';
                    }
                    str +=ch;
                }

                data["map"] = mapa_;
                //answer_build.Value(data);

        } else if(sr.type == "Route"){
            //LogDuration lg2("Answer Route");
            data["request_id"] = sr.id;
            //graph::Router<double> router(transport_router_->GetGraph());
            auto router_info = router_ptr_->BuildRoute(transport_router_->GetRouterByStop(sr.from.value()).bus_wait_start, transport_router_->GetRouterByStop(sr.to.value()).bus_wait_start);
                if(router_info){
                   // LogDuration lgRouter("Edges ");
                    Array stops;
                    using transport_catalogue::detail::router::StopEdge;
                    using transport_catalogue::detail::router::BusEdge;
                    for(graph::EdgeId stopbus:router_info->edges){
                        Dict dict_route;
                        std::variant<StopEdge, BusEdge> edge = transport_router_->GetEdge(stopbus);
                        if(std::holds_alternative<StopEdge>(edge)){
                            StopEdge se = std::get<StopEdge>(edge);
                            dict_route["type"] = std::string("Wait");
                            dict_route["time"] = se.time;
                            dict_route["stop_name"] = se.name;

                        } else if(std::holds_alternative<BusEdge>(edge)){
                            BusEdge be = std::get<BusEdge>(edge);
                            dict_route["type"] = std::string("Bus");
                            dict_route["bus"] = std::string(be.bus_name);
                            dict_route["time"] = be.time;
                            dict_route["span_count"] = int(be.span_count);
                        }
                        stops.push_back(dict_route);
                    }
                    data["items"] = stops;
                    data["total_time"] = router_info->weight;
            } else {
                    data["error_message"] = std::string("not found");
                }
        }
        else {
            throw "no answer";
        }
        array_data.push_back(data);

    }
   answer_build.Value(array_data);
   Document doc(answer_build.Build());
   Print(doc, out);

}

TransportCatalog& JSONr::GetTransportCataloge(){
    //LogDuration ld("GetTransportCatalogFromJSONr");
    return transport_catalogue_;
}

RenderSettings& JSONr::GetRenderSettings(){
    return render_settings_;
}


void JSONr::SetTransportRouter(std::unique_ptr<TransportRouter> tr){
    transport_router_ = std::move(tr);
    router_ptr_ = std::make_unique<graph::Router<double>>(transport_router_->GetGraph());
}

/*
Array JSONr::CreateAnsRoute(Dict& dict){

}
*/
}//end namespace json
}//end namespace detail
}//end namespace transport_catalogue
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
