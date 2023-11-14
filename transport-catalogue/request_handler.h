#pragma once
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "geo.h"

using namespace transport_catalogue;
using namespace map_renderer;

namespace request_handler {

using BusPoints = std::vector<std::pair<std::string_view, std::vector<geo::Coordinates>>>;

class RequestHandler {
public:

    RequestHandler(TransportCatalogue& db, const MapRenderer& map_renderer_);

    std::vector<geo::Coordinates> get_stops_coordinates() const;
    void render_map(std::ostream& out) const;

private:
    TransportCatalogue& db_;
    const MapRenderer& map_renderer_;
};
}
