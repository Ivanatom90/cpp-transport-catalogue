#include "json_reader.h"


namespace transport_catalogue {
namespace detail {
namespace json {

Stop parse_node_stop(Node& node) {
    Stop stop;
    Dict stop_node;

    if (node.is_map()) {
        stop_node = node.as_map();
        stop.name = stop_node.at("name").as_string();
        stop.latitude = stop_node.at("latitude").as_double();
        stop.longitude = stop_node.at("longitude").as_double();
    }

    return stop;
}

std::vector<Distance> parse_node_distances(Node& node, TransportCatalogue& catalogue) {
    std::vector<Distance> distances;
    Dict stop_node;
    Dict stop_road_map;
    std::string begin_name;
    std::string last_name;
    int distance;

    if (node.is_map()) {
        stop_node = node.as_map();
        begin_name = stop_node.at("name").as_string();

        try {
            stop_road_map = stop_node.at("road_distances").as_map();

            for (auto [key, value] : stop_road_map) {
                last_name = key;
                distance = value.as_int();
                distances.push_back({catalogue.get_stop(begin_name),
                                     catalogue.get_stop(last_name), distance});
            }

        } catch(...) {
          //  std::cout << "invalide road" << std::endl;
        }
    }

    return distances;
}

Bus parse_node_bus(Node& node, TransportCatalogue& catalogue) {
    Bus bus;
    Dict bus_node;
    Array bus_stops;

    if (node.is_map()) {
        bus_node = node.as_map();
        bus.name = bus_node.at("name").as_string();
        bus.is_roundtrip = bus_node.at("is_roundtrip").as_bool();

        try {
            bus_stops = bus_node.at("stops").as_array();

            for (Node stop : bus_stops) {
                bus.stops.push_back(catalogue.get_stop(stop.as_string()));
            }
            bus.stops_not_all = bus.stops;
            if (!bus.is_roundtrip) {
                size_t size = bus.stops.size() - 1;

                for (size_t i = size; i > 0; i--) {
                    bus.stops.push_back(bus.stops[i-1]);
                }

            }

        } catch(...) {
          //  std::cout << "base_requests: bus: stops is empty" << std::endl;
        }
    }

    return bus;
}

void parse_node_base(const Node& root, TransportCatalogue& catalogue){
    Array base_requests;
    Dict req_map;
    Node req_node;

    std::vector<Node> buses;
    std::vector<Node> stops;

    if (root.is_array()) {
        base_requests = root.as_array();

        for (Node& node : base_requests) {
            if (node.is_map()) {
                req_map = node.as_map();

                try {
                    req_node = req_map.at("type");

                    if (req_node.is_string()) {

                        if (req_node.as_string() == "Bus") {
                            buses.push_back(req_map);
                        } else if (req_node.as_string() == "Stop") {
                            stops.push_back(req_map);
                        } else {
                           // std::cout << "base_requests are invalid";
                        }
                    }

                } catch(...) {
                  //  std::cout << "base_requests does not have type value";
                }
            }
        }

        for (auto stop : stops) {
            catalogue.add_stop(parse_node_stop(stop));
        }

        for (auto stop : stops) {
            catalogue.add_distance(parse_node_distances(stop, catalogue));
        }

        for (auto bus : buses) {
            catalogue.add_bus(parse_node_bus(bus, catalogue));
        }

    } else {
      //  std::cout << "base_requests is not an array";
    }
}

void parce_node_stat(const Node& node, std::vector<StatRequest>& stat_request){
    Array stat_requests;
    Dict req_map;
    StatRequest req;

    if (node.is_array()) {
        stat_requests = node.as_array();

        for (Node req_node : stat_requests) {

            if (req_node.is_map()) {
                req_map = req_node.as_map();
                req.id = req_map.at("id").as_int();
                req.type = req_map.at("type").as_string();
                if(req.type != "Map") {req.name = req_map.at("name").as_string();}
                stat_request.push_back(req);
            }

        }
    } else {
        std::cout << "base_requests is not array";
    }
}

void parce_node_render(const Node& node, map_renderer::RenderSettings& rend_set){
    Dict rend_map;
    Array bus_lab_offset;
    Array stop_lab_offset;
    Array arr_color;
    Array arr_palette;
    uint8_t red_;
    uint8_t green_;
    uint8_t blue_;
    double opacity_;

    if (node.is_map()) {
        rend_map = node.as_map();

        try {
            rend_set.width_ = rend_map.at("width").as_double();
            rend_set.height_ = rend_map.at("height").as_double();
            rend_set.padding_ = rend_map.at("padding").as_double();
            rend_set.line_width_ = rend_map.at("line_width").as_double();
            rend_set.stop_radius_ = rend_map.at("stop_radius").as_double();
            rend_set.bus_label_font_size_ = rend_map.at("bus_label_font_size").as_int();

            if (rend_map.at("bus_label_offset").is_array()) {
                bus_lab_offset = rend_map.at("bus_label_offset").as_array();
                rend_set.bus_label_offset_ = std::make_pair(bus_lab_offset[0].as_double(),
                                                            bus_lab_offset[1].as_double());
            }

            rend_set.stop_label_font_size_ = rend_map.at("stop_label_font_size").as_int();

            if (rend_map.at("stop_label_offset").is_array()) {
                stop_lab_offset = rend_map.at("stop_label_offset").as_array();
                rend_set.stop_label_offset_ = std::make_pair(stop_lab_offset[0].as_double(),
                                                             stop_lab_offset[1].as_double());
            }

            if (rend_map.at("underlayer_color").is_string()) {
                rend_set.underlayer_color_ = svg::Color(rend_map.at("underlayer_color").as_string());
            } else if (rend_map.at("underlayer_color").is_array()) {
                arr_color = rend_map.at("underlayer_color").as_array();
                red_ = arr_color[0].as_int();
                green_ = arr_color[1].as_int();
                blue_ = arr_color[2].as_int();

                if(arr_color.size() == 4){
                    opacity_ = arr_color[3].as_double();
                    rend_set.underlayer_color_ = svg::Color(svg::Rgba(red_,
                                                                      green_,
                                                                      blue_,
                                                                      opacity_));
                } else if (arr_color.size() == 3) {
                    rend_set.underlayer_color_ = svg::Color(svg::Rgb(red_, green_, blue_));
                }
            }

            rend_set.underlayer_width_ = rend_map.at("underlayer_width").as_double();

            if (rend_map.at("color_palette").is_array()) {
                arr_palette = rend_map.at("color_palette").as_array();

                for (Node color_palette : arr_palette) {
                    if (color_palette.is_string()) {
                        rend_set.color_palette_.push_back(svg::Color(color_palette.as_string()));
                    } else if (color_palette.is_array()) {
                        arr_color = color_palette.as_array();
                        red_ = arr_color[0].as_int();
                        green_ = arr_color[1].as_int();
                        blue_ = arr_color[2].as_int();

                        if (arr_color.size() == 4) {
                            opacity_ = arr_color[3].as_double();
                            rend_set.color_palette_.push_back(svg::Color(svg::Rgba(red_, green_, blue_, opacity_)));

                        } else if (arr_color.size() == 3) {
                            rend_set.color_palette_.push_back(svg::Color(svg::Rgb(red_,green_,blue_)));
                        }
                    }
                }
            }
        } catch(...) {
         //   std::cout << "unable to parsse init settings";
        }

    } else {
      //  std::cout << "render_settings is not map";
    }
}

void parse_node(const Node& root, TransportCatalogue& catalogue, [[maybe_unused]] std::vector<StatRequest>& stat_request, map_renderer::RenderSettings& render_settings){

Dict root_dictionary;

    if (root.is_map()) {
        root_dictionary = root.as_map();

        try {
            parse_node_base(root_dictionary.at("base_requests"), catalogue);
        } catch(...) {
          //  std::cout << "base_requests is empty";
        }

        try {
            parce_node_stat(root_dictionary.at("stat_requests"), stat_request);
        } catch(...) {
        //    std::cout << "stat_requests is empty";
        }

       try {
            parce_node_render(root_dictionary.at("render_settings"), render_settings);
        } catch(...) {
          //  std::cout << "render_settings is empty";
        }

    } else {
      //  std::cout << "root is not map";
    }
}

void parse_node(const Node& root, TransportCatalogue& catalogue, [[maybe_unused]] std::vector<StatRequest>& stat_request){
Dict root_dictionary;

    if (root.is_map()) {
        root_dictionary = root.as_map();

        try {
            parse_node_base(root_dictionary.at("base_requests"), catalogue);
        } catch(...) {
         //   std::cout << "base_requests is empty";
        }

        try {
            parce_node_stat(root_dictionary.at("stat_requests"), stat_request);
        } catch(...) {
        //    std::cout << "stat_requests is empty";
        }
    }

    else {
      //  std::cout << "root is not map";
    }
}

void parse(Document& document, TransportCatalogue& catalogue, std::vector<StatRequest>& stat_request, map_renderer::RenderSettings& render_settings){
    parse_node(document.get_root(),
               catalogue,
               stat_request,
               render_settings);
}

void parse(Document& document, TransportCatalogue& catalogue, std::vector<StatRequest>& stat_request){
    parse_node(document.get_root(),
               catalogue,
               stat_request
               );
}

Node execute_make_node_stop(int id_request, StopQueryResult stop_info){
    Dict result;
    Array buses;
    std::string str_not_found = "not found";

    if (stop_info.not_found) {
        result.emplace("request_id", Node(id_request));
        result.emplace("error_message", Node(str_not_found));
    } else {
        result.emplace("request_id", Node(id_request));

        for (std::string bus_name : stop_info.buses_name) {
            buses.push_back(Node(bus_name));
        }

        result.emplace("buses", Node(buses));
    }

    return Node(result);
}

Node execute_make_node_bus(int id_request, BusQueryResult bus_info){
    Dict result;
    std::string str_not_found = "not found";

    if (bus_info.not_found) {
        result.emplace("request_id", Node(id_request));
        result.emplace("error_message", Node(str_not_found));
    } else {
        result.emplace("request_id", Node(id_request));
        result.emplace("curvature", Node(bus_info.curvature));
        result.emplace("route_length", Node(bus_info.route_length));
        result.emplace("stop_count", Node(bus_info.stops_on_route));
        result.emplace("unique_stop_count", Node(bus_info.unique_stops));
    }

    return Node(result);
}

Node execute_make_node_map(int id_request, std::string& out_svg){
    Dict result;

    std::string str_not_found = "not found";
        result.emplace("request_id", Node(id_request));
        result.emplace("map", Node(out_svg));

    return Node(result);
}

Document execute_queries([[maybe_unused]] TransportCatalogue& catalogue, std::vector<StatRequest>& stat_requests, [[maybe_unused]]request_handler::RequestHandler& Rh){
    std::vector<Node> result_request;

    for (StatRequest req : stat_requests) {

        if (req.type == "Stop") {
            result_request.push_back(execute_make_node_stop(req.id, stop::query(catalogue, req.name)));
        } else if (req.type == "Bus") {
            result_request.push_back(execute_make_node_bus(req.id,bus::query(catalogue, req.name)));
        } else if (req.type == "Map") {
            std::ostringstream osrs;
            Rh.render_map(osrs);
            std::string str (osrs.str());
            result_request.push_back(execute_make_node_map(req.id, str));
        }
    }

    return Document{Node(result_request)};
}

}//end namespace json
}//end namespace detail
}//end namespace transport_catalogue
