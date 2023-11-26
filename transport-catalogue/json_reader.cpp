#include "json_reader.h"


namespace transport_catalogue {
namespace detail {
namespace json {

JSONr::JSONr(Document& doc, std::ostream& ou): document_(doc), out(ou) {}

void JSONr::ParseReq(){
    Dict main_node;
    if (document_.get_root().is_map()){
        main_node = document_.get_root().as_map();
            JSONr::ParseBasReq(main_node.at("base_requests"));
            JSONr::ParseRenderSettings(main_node.at("render_settings"));
            JSONr::ParseStatReq(main_node.at("stat_requests"));
    }
}

void JSONr::ParseBasReq(const Node& BasicRegNode){

    std::vector<Node> buses_nodes;
    std::vector<Node> stops_nodes;

    if (BasicRegNode.is_array()){
            const Array& node_base = BasicRegNode.as_array();
            for(const Node& node:node_base){
                if(node.is_map()){
                    const Dict&  reg_node = node.as_map();
                    if(reg_node.at("type").as_string() == "Bus"){
                        buses_nodes.push_back(node);
                    } else if(reg_node.at("type").as_string() == "Stop"){
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

    if(bus.is_map()){
        bus_node = bus.as_map();
        bus_js.name = bus_node.at("name").as_string();
        for(const Node& n:bus_node.at("stops").as_array()){
            bus_js.route.push_back(n.as_string());
        }
        bus_js.is_roudtrip = bus_node.at("is_roundtrip").as_bool();
    } else {
        throw "not create BusJ";
    }
    buses_j_.push_back(bus_js);

}

void JSONr::CreateStopJ(const Node& stop){
    Stop_J stop_js;
    Dict stop_node;
    if(stop.is_map()){
        stop_node = stop.as_map();
        stop_js.name = stop_node.at("name").as_string();
        stop_js.latitude = stop_node.at("latitude").as_double();
        stop_js.longitude = stop_node.at("longitude").as_double();
        for(const auto& dist:stop_node.at("road_distances").as_map()  ){

                std::string key = dist.first;
                stop_js.dist_to[dist.first] = dist.second.as_int();

        }
    stops_j_.push_back(stop_js);
    } else {
        throw "not stop reqvest";
    }
}

void JSONr::ParseStatReq(const Node& BasicRegnode){
    StatRequest sr;
    if(BasicRegnode.is_array()){
       for(const Node& node:BasicRegnode.as_array()){
           try{
               const Dict& dict = node.as_map();
               if(dict.at("type").as_string() != "Map"){
                   sr.id = dict.at("id").as_int();
                   sr.name = dict.at("name").as_string();
                   sr.type = dict.at("type").as_string();
               } else {
                   sr.id = dict.at("id").as_int();
                   sr.type = dict.at("type").as_string();
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
    if(color.is_array()){
        if(color.as_array().size() == 4){
            const Array& rgba = color.as_array();
            return svg::Rgba(rgba[0].as_int(), rgba[1].as_int(),rgba[2].as_int(), rgba[3].as_double());
        } else if(color.as_array().size() == 3){
            const Array& rgb = color.as_array();
            return svg::Rgb(rgb[0].as_int(), rgb[1].as_int(),rgb[2].as_int());
        }
    }
        return color.as_string();

}

void JSONr::ParseRenderSettings(const Node& BasicRegnode){
    RenderSettings& rs = render_settings_;
    if(BasicRegnode.is_map()){
            const Dict& dict = BasicRegnode.as_map();
            rs.width = dict.at("width").as_double();
            rs.height = dict.at("height").as_double();
            rs.padding = dict.at("padding").as_double();
            rs.stop_radius = dict.at("stop_radius").as_double();
            rs.line_width = dict.at("line_width").as_double();
            rs.bus_label_font_size = dict.at("bus_label_font_size").as_int();
            rs.bus_label_offset = {dict.at("bus_label_offset").as_array()[0].as_double(),dict.at("bus_label_offset").as_array()[1].as_double()} ;
            rs.stop_label_offset = {dict.at("stop_label_offset").as_array()[0].as_double(),dict.at("stop_label_offset").as_array()[1].as_double()};
            rs.underlayer_width = dict.at("underlayer_width").as_double();
            rs.underlayer_color = DetermineColor(dict.at("underlayer_color"));
            rs.stop_label_font_size = dict.at("stop_label_font_size").as_int();
            const Array& palette = dict.at("color_palette").as_array();
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
}

void JSONr::CreateAnswer(){

    for(const StatRequest& sr:stat_reqs_){
        Answer answer;

        if(sr.type == "Bus"){

             if(transport_catalogue_.BusRoudeExist(sr.name)){
                const BusRoute* br =transport_catalogue_.GetBusRoude(sr.name);
                answer["curvature"] = br->curvature;
                answer["request_id"] = sr.id;
                answer["route_length"] = int(br->distance);
                answer["unique_stop_count"] = br->unique_route_number;
                (br->is_roundtrip)
                    ? answer["stop_count"] = int(br->route.size())
                    : answer["stop_count"] = (int(br->route.size())*2)-1;
             } else {
                 answer["request_id"] = sr.id;
                 answer["error_message"] = "not found";
             }
        } else if(sr.type == "Stop"){
            answer["request_id"] = sr.id;
            if(transport_catalogue_.BusStopExist(sr.name)){
                std::vector<std::string> buses;
                const BusStop* bs = transport_catalogue_.GetBusStop(sr.name);
                for(const std::string& bus_number:bs->buses_numbers){
                    buses.push_back(bus_number);
                }
                answer["buses"]=buses;
            } else {
                answer["error_message"] = "not found";
            }
        } else if(sr.type == "Map"){
                answer["request_id"] =sr.id;
                std::string str = "";
                for(const char ch:mapa_){
                    if(ch == '"' || ch == '\\'){
                        str += '\\';
                    }
                    str +=ch;
                }
                answer["map"] = str;
        }
        else {
            throw "no answer";
        }

      answers_.push_back(answer);
    }

}

struct AnswerPrinter{
    void operator()(std::string str){
        ou<<"  \""<<str<<"\"";
    }
    void operator()(int num){
        ou<<" "<<num;
    }
    void operator()(double num){
        ou<<" "<<num;
    }
    void operator()(std::vector<std::string> vec){
        ou<<"[ ";
        bool first = true;
        for(const std::string& str:vec){
            (first)
                    ? ou<<'\n'<<"       \""<<str<<"\""
                    : ou<<", "<<'\n'<<"      \""<<str<<"\"";
            first=false;
        }
        ou<<" ]";
    }

    std::ostream& ou;
};


void JSONr::PrintAnswer(){
    CreateAnswer();
    out<<"  ["<<'\n';
    bool first = true;
    for(const Answer& dict:answers_){
        (first)
                ? out<<"    {"<<std::endl
                : out<<","<<'\n'<<"    {"<<std::endl;
        bool first2 = true;
        for(const auto& k:dict){
            (first2)? out<<" " : out<<", "<<std::endl;
            out<<" \""<<k.first<<"\": ";
            std::visit(AnswerPrinter{out}, k.second);
            first2 =false;
        }
        out<<"    }";
        first = false;
    }
    out<<'\n'<<"  ]"<<std::endl;
}


TransportCatalog& JSONr::GetTransportCataloge(){
    return transport_catalogue_;
}

RenderSettings& JSONr::GetRenderSettings(){
    return render_settings_;
}

/*
void JSONr::CreateAnsvertoReq(){
    out<<'['<<'\n';
    size_t size_req = stat_reqs_.size();
    for(const StatRequest& sr:stat_reqs_){
        if(sr.type == "Bus"){
            AnswertoRequestBus arb;
            if(!transport_catalogue_.BusRoudeExist(sr.name)){
                out<<"  {"<<'\n'<<R"(    "request_id": )"<<sr.id<<","<<'\n'
                  << R"(    "error_message": )"<< R"("not found")"<<'\n'
                  <<"  }"<<std::endl;
            } else {
            const BusRoute* br = transport_catalogue_.GetBusRoude(sr.name);
                arb.curvature = br->curvature;
                arb.request_id = sr.id;
                arb.route_length = br->distance;
                arb.stop_count = br->route.size()+1;
                arb.unique_stop_count = br->unique_route_number;
                answer_bus_.push_back(arb);
                out<<arb;
                  }
        } else if(sr.type == "Stop"){
            if(!transport_catalogue_.BusStopExist(sr.name)){
                out<<"  {"<<'\n'<<R"(    "request_id": )"<<sr.id<<","<<'\n'
                  << R"(    "error_message": )"<< R"("not found")"<<'\n'
                  <<"  }"<<std::endl;
            } else {

            AnswertoRequestStop ars;
            const BusStop* bs = transport_catalogue_.GetBusStop(sr.name);

                ars.request_id = sr.id;
                ars.buses_names = bs->buses_numbers;
            out<<ars;
        }
        } else {
            throw "no answer";
        }
        if(--size_req !=0){out<<", ";}

    }
    out<<']'<<std::endl;
}

std::ostream& operator<<(std::ostream& ou, AnswertoRequestBus& arb){
    ou<<"  {"<<'\n';
    ou<<R"(    "curvature": )"<<arb.curvature<<","<<'\n'
     <<R"(    "request_id": )"<<arb.request_id<<","<<'\n'
    <<R"(    "route_length": )" <<arb.route_length<<","<<'\n'
    <<R"(    "stop_count": )"<<arb.stop_count<<","<<'\n'
    <<R"(    "unique_stop_count": )"<<arb.unique_stop_count<<'\n';
    ou<<"  }"<<std::endl;
    return ou;
}

std::ostream& operator<<(std::ostream& ou, AnswertoRequestStop& ars){
    ou<<"  {"<<'\n';
        ou<<R"(    "buses": [)"<<'\n';
        bool first = true;
        for(const std::string& bus:ars.buses_names){
            (first)? ou<<"    "<<bus : ou<<", "<<'\n'<<"    "<<bus;
            first = false;
        }
        ou<<'\n'<<"],"<<'\n';
        ou<<R"(    "request_id": )"<<ars.request_id<<'\n';
    ou<<"  }"<<std::endl;
    return ou;
}
*/

}//end namespace json
}//end namespace detail
}//end namespace transport_catalogue
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
