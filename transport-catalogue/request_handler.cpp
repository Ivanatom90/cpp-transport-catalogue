#include "request_handler.h"
#include <set>
#include <map>

namespace request_handler {

RequestHandler::RequestHandler(TransportCatalogue& db,const MapRenderer& renderer) : db_(db),
                                                                                     map_renderer_(renderer) {}

std::vector<geo::Coordinates> RequestHandler::get_stops_coordinates() const {

    std::vector<geo::Coordinates> stops_coordinates;
    const auto &stops = db_.Get_stops();

    for (const auto& stop : stops) {
            if(stop.buses.size()){
                    geo::Coordinates coordinates(stop.latitude, stop.longitude);
                    stops_coordinates.push_back(coordinates);
            }
    }
    return stops_coordinates;
}

void RequestHandler::render_map(std::ostream& out) const {
    svg::Document document;
    //[[maybe_unused]] auto sphere_projector = map_renderer_.get_sphere_projector(get_stops_coordinates());
    const SphereProjector sphere_projector = map_renderer_.get_sphere_projector(get_stops_coordinates());
    //std::vector<geo::Coordinates> all_buses_stops_cord = get_stops_coordinates();
    std::vector<const transport_catalogue::Stop*> stops_with_bus;
    std::set<std::string> buses_name;
    std::map<std::string, int> buses_palette; //что - то гдето тут
    std::vector<geo::Coordinates> stops_geo_coords;
    std::vector<svg::Point> stops_points_;
    int palette_size = map_renderer_.get_palette_size();
    int palette_index = 0;
    if (palette_size == 0) {
     //   std::cout << "color palette is empty";
        return;
    }
    for(const Stop& stop : db_.Get_stops()){
        if(stop.buses.size()){
            stops_with_bus.push_back(&stop);
        }
    }

    std::sort(stops_with_bus.begin(), stops_with_bus.end(),[](const Stop* s1, const Stop* s2){
        return s1->name<s2->name;
    });

    for(const Bus& bus : db_.Get_Buses()){
        buses_name.insert(bus.name);
    }

    for(const std::string& name:buses_name){
        buses_palette[name] = palette_index;
        (palette_index < palette_size-1) ? palette_index++ : palette_index = 0;
    }

    std::vector<const Bus*> buses;
    for (const Bus& bus : db_.Get_Buses()){
        buses.push_back(&bus);
    }

    std::sort(buses.begin(), buses.end(),[](const Bus* b1, const Bus* b2){
        return b1->name < b2->name;
    });


    for (const Bus* bus : buses) {

        if(bus->stops.size() ==0){continue;}
        for (Stop* stop : bus->stops) {
            geo::Coordinates coordinates = {stop->latitude, stop->longitude};
            stops_geo_coords.push_back(coordinates);
        }

        svg::Polyline bus_line;
        svg::Point screen_coord;

        for (const geo::Coordinates& point : stops_geo_coords) {
            screen_coord  = sphere_projector(point);
            bus_line.AddPoint(screen_coord);
        }

            map_renderer_.set_line_properties(bus_line, buses_palette.at(bus->name));
            document.Add(bus_line);
        stops_geo_coords.clear();
    }

    for (const Bus* bus : buses){
         if(!bus->stops.size()){continue;}
         svg::Text text;
             geo::Coordinates cord (bus->stops.front()->latitude, bus->stops.front()->longitude);
             svg::Point cord_offset (map_renderer_.get_render_settings().bus_label_offset_.first, map_renderer_.get_render_settings().bus_label_offset_.second);
             int index_palette = buses_palette.at(bus->name);
             text.SetPosition(sphere_projector(cord))
                     .SetOffset(cord_offset)
                     .SetFontSize(map_renderer_.get_render_settings().bus_label_font_size_)
                     .SetFontFamily("Verdana")
                     .SetFontWeight("bold")
                     .SetData(bus->name)
                     .SetFillColor(map_renderer_.get_render_settings().underlayer_color_)
                     .SetStrokeColor(map_renderer_.get_render_settings().underlayer_color_)
                     .SetStrokeWidth(map_renderer_.get_render_settings().underlayer_width_)
                     .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                     .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
             document.Add(text);

             svg::Text text_t;
             text_t.SetPosition(sphere_projector(cord))
                     .SetOffset(cord_offset)
                     .SetFontSize(map_renderer_.get_render_settings().bus_label_font_size_)
                     .SetFontFamily("Verdana")
                     .SetFontWeight("bold")
                     .SetData(bus->name)
                     .SetFillColor(map_renderer_.get_color(index_palette));

             document.Add(text_t);

             if(!bus->is_roundtrip && (bus->stops_not_all.front() !=bus->stops_not_all.back())){
                 svg::Text text2;
                     //const auto& render_settings = map_renderer_.get_render_settings();
                     geo::Coordinates cord (bus->stops_not_all.back()->latitude, bus->stops_not_all.back()->longitude);
                     svg::Point cord_offset (map_renderer_.get_render_settings().bus_label_offset_.first, map_renderer_.get_render_settings().bus_label_offset_.second);
                     int index_palette = buses_palette.at(bus->name);
                     text2.SetPosition(sphere_projector(cord))
                             .SetOffset(cord_offset)
                             .SetFontSize(map_renderer_.get_render_settings().bus_label_font_size_)
                             .SetFontFamily("Verdana")
                             .SetFontWeight("bold")
                             .SetData(bus->name)
                             .SetFillColor(map_renderer_.get_render_settings().underlayer_color_)
                             .SetStrokeColor(map_renderer_.get_render_settings().underlayer_color_)
                             .SetStrokeWidth(map_renderer_.get_render_settings().underlayer_width_)
                             .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                             .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                     document.Add(text2);

                     svg::Text text_t2;
                     text_t2.SetPosition(sphere_projector(cord))
                             .SetOffset(cord_offset)
                             .SetFontSize(map_renderer_.get_render_settings().bus_label_font_size_)
                             .SetFontFamily("Verdana")
                             .SetFontWeight("bold")
                             .SetData(bus->name)
                             .SetFillColor(map_renderer_.get_color(index_palette));

                     document.Add(text_t2);
             }
         }

        for(const auto& stop:stops_with_bus){
               svg::Circle circle;
               circle.SetCenter(sphere_projector(geo::Coordinates(stop->latitude, stop->longitude))).SetRadius(map_renderer_.get_render_settings().stop_radius_).SetFillColor("white");
               document.Add(circle);
        }

        for(const auto& stop:stops_with_bus){

            svg::Text text;
                //const auto& render_settings = map_renderer_.get_render_settings();
                geo::Coordinates cord (stop->latitude, stop->longitude);
                svg::Point cord_offset (map_renderer_.get_render_settings().stop_label_offset_.first, map_renderer_.get_render_settings().stop_label_offset_.second);
                text.SetPosition(sphere_projector(cord))
                        .SetOffset(cord_offset)
                        .SetFontSize(map_renderer_.get_render_settings().stop_label_font_size_)
                        .SetFontFamily("Verdana")
                        .SetData(stop->name)
                        .SetFillColor(map_renderer_.get_render_settings().underlayer_color_)
                        .SetStrokeColor(map_renderer_.get_render_settings().underlayer_color_)
                        .SetStrokeWidth(map_renderer_.get_render_settings().underlayer_width_)
                        .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                        .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                document.Add(text);

                svg::Text text_t;
                text_t.SetPosition(sphere_projector(cord))
                        .SetOffset(cord_offset)
                        .SetFontSize(map_renderer_.get_render_settings().stop_label_font_size_)
                        .SetFontFamily("Verdana")
                        .SetData(stop->name)
                        .SetFillColor("black");

                document.Add(text_t);
        }

    document.Render(out);
}

}//end namespace request_handler

