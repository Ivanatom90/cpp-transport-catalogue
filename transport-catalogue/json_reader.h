#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "domain.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "iostream"

namespace transport_catalogue {
namespace detail {
namespace json {

using namespace transport_catalogue::detail::stop;
using namespace transport_catalogue::detail::bus;

struct StatRequest {
    int id;
    std::string type;
    std::string name;
};

void parse_node_base(const Node& root, TransportCatalogue& catalogue);
void parse_node_stat(const Node& root, std::vector<StatRequest>& stat_request);
void parse_node_render(const Node& node, map_renderer::RenderSettings& render_settings);
void parse_node(const Node& root, TransportCatalogue& catalogue, std::vector<StatRequest>& stat_request, map_renderer::RenderSettings& render_settings);
void parse_node(const Node& root, TransportCatalogue& catalogue, std::vector<StatRequest>& stat_request);
void parse(Document& doc, TransportCatalogue& catalogue, std::vector<StatRequest>& stat_request, map_renderer::RenderSettings& render_settings);
void parse(Document& doc, TransportCatalogue& catalogue, std::vector<StatRequest>& stat_request);
Stop parse_node_stop(Node& node);
Bus parse_node_bus(Node& node, TransportCatalogue& catalogue);
std::vector<Distance> parse_node_distances(Node& node, TransportCatalogue& catalogue);

Node execute_make_node_stop(int id_request, StopQueryResult query_result);
Node execute_make_node_bus(int id_request, BusQueryResult query_result);
Node execute_make_node_map(int id_request, std::string& out_svg);
//Document execute_queries(TransportCatalogue& catalogue, std::vector<StatRequest>& stat_requests);
Document execute_queries([[maybe_unused]] TransportCatalogue& catalogue, std::vector<StatRequest>& stat_requests, [[maybe_unused]]request_handler::RequestHandler& Rh);


}//end namespace json
}//end namespace detail
}//end namespace transport_catalogue
