#include "map_renderer.h"

namespace map_renderer {


MapRenderer::MapRenderer(RenderSettings&& render_settings, BusRoutes& br):bus_routes_(br) {
    render_settings_ = std::move(render_settings);
    sphere_projector_ = CreateSphereProjector();
    CreateBusStopsAll();
    CreateRoutes();
}


SphereProjector MapRenderer::CreateSphereProjector(){
    std::vector<detail::geo::Coordinates> cordinates;

    for(const std::pair<std::string, std::vector<BusStopForMap>>& bus : bus_routes_){
       for(const BusStopForMap& bsfm : bus.second){
           cordinates.push_back(bsfm.coordinates);
       }
    }
    return SphereProjector {cordinates.begin(), cordinates.end(), render_settings_.width, render_settings_.height, render_settings_.padding};

}

void MapRenderer::CreateBusStopsAll(){
    for(const std::pair<std::string, std::vector<BusStopForMap>>& bus : bus_routes_){
        for(const BusStopForMap& stop : bus.second){
            bool have_stop = std::count_if(bus_stops_all_.begin(), bus_stops_all_.end(), [&stop](const BusStopForMap& a){
                return a.name == stop.name;
            });
            if(!have_stop){
                bus_stops_all_.push_back(stop);
            }
        }
    }

    std::sort(bus_stops_all_.begin(), bus_stops_all_.end(), [](const BusStopForMap& a, const BusStopForMap& b){
        return a.name<b.name;
    });
}

void MapRenderer::CreateRoutes(){
    std::vector<std::unique_ptr<svg::Drawable>> pictures;
    size_t i = 0;
        for(const std::pair<std::string, std::vector<BusStopForMap>>& bus : bus_routes_){
            document_map_.Add(CreateRouteline(bus.second, render_settings_.color_palette[i]));
           (i < render_settings_.color_palette.size()-1)? i++ : i =0;
        }

        i =0;
        for(const std::pair<std::string, std::vector<BusStopForMap>>& bus : bus_routes_){
            CreateRouteTitle(bus.first, bus.second, render_settings_.color_palette[i]);
           (i < render_settings_.color_palette.size()-1)? i++ : i =0;
        }

        CreateStopsSymbols();
        CreateStopsTitles();

}

svg::Polyline MapRenderer::CreateRouteline(const std::vector<BusStopForMap>& bus_route, svg::Color color){
    svg::Polyline line_bus;
    std::vector<detail::geo::Coordinates> cordinates;

    for(const BusStopForMap& bsfm : bus_route){
        cordinates.push_back(bsfm.coordinates);

    }

    if(!bus_route[0].is_roundtrip){
        for(int i = bus_route.size()-2; i>=0; i--){
           cordinates.push_back(bus_route[i].coordinates);
        }
    }


    for(detail::geo::Coordinates& cord : cordinates){
        svg::Point screen_coord = sphere_projector_(cord);
        line_bus.AddPoint(screen_coord);
    }
    line_bus.SetStrokeColor(color)
            .SetFillColor("none")
            .SetStrokeWidth(render_settings_.line_width)
            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

return line_bus;

}

void MapRenderer::CreateRouteTitle(const std::string& bus_name, const std::vector<BusStopForMap>& stops, svg::Color color){
    svg::Text underlayer_text;
    svg::Text text;
    const RenderSettings& rs = render_settings_;
    svg::Point cord_ofset = {render_settings_.bus_label_offset.first, render_settings_.bus_label_offset.second};

    underlayer_text.SetPosition(sphere_projector_(stops[0].coordinates))
            .SetOffset(cord_ofset)
            .SetFontSize(rs.bus_label_font_size)
            .SetFontFamily("Verdana")
            .SetFontWeight("bold")
            .SetData(bus_name)
            .SetFillColor(rs.underlayer_color)
            .SetStrokeColor(rs.underlayer_color)
            .SetStrokeWidth(rs.underlayer_width)
            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    document_map_.Add(underlayer_text);

    text.SetPosition(sphere_projector_(stops[0].coordinates))
            .SetOffset(cord_ofset)
            .SetFontSize(rs.bus_label_font_size)
            .SetFontFamily("Verdana")
            .SetFontWeight("bold")
            .SetData(bus_name)
            .SetFillColor(color);
    document_map_.Add(text);

    if(!stops[0].is_roundtrip && stops[0].name != stops.back().name){
        underlayer_text.SetPosition(sphere_projector_(stops.back().coordinates));
        document_map_.Add(underlayer_text);

        text.SetPosition(sphere_projector_(stops.back().coordinates));
        document_map_.Add(text);
    }

}

void MapRenderer::CreateStopsSymbols(){

    for(const BusStopForMap& bsfm : bus_stops_all_){
        svg::Circle stop;
        stop.SetCenter(sphere_projector_(bsfm.coordinates))
                .SetRadius(render_settings_.stop_radius)
                .SetFillColor("white");
        document_map_.Add(stop);
    }



}

void MapRenderer::CreateStopsTitles(){
    svg::Text underlayer_text;
    for(const BusStopForMap& stop : bus_stops_all_){
        svg::Text underlayer_text;
        svg::Text text;
        const RenderSettings& rs = render_settings_;
        svg::Point cord_ofset = {render_settings_.stop_label_offset.first, render_settings_.stop_label_offset.second};

        underlayer_text.SetPosition(sphere_projector_(stop.coordinates))
                .SetOffset(cord_ofset)
                .SetFontSize(rs.stop_label_font_size)
                .SetFontFamily("Verdana")
                .SetData(stop.name)
                .SetFillColor(rs.underlayer_color)
                .SetStrokeColor(rs.underlayer_color)
                .SetStrokeWidth(rs.underlayer_width)
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        document_map_.Add(underlayer_text);


        text.SetPosition(sphere_projector_(stop.coordinates))
                .SetOffset(cord_ofset)
                .SetFontSize(rs.stop_label_font_size)
                .SetFontFamily("Verdana")
                .SetData(stop.name)
                .SetFillColor("black");
        document_map_.Add(text);

    }

}

void MapRenderer::PrintDocument(std::ostream& out = std::cout){
    document_map_.Render(out);
}

svg::Point SphereProjector::operator()(detail::geo::Coordinates coords) const {
    return {
        (coords.longitude - min_lon_) * zoom_coeff_ + padding_,
        (max_lat_ - coords.latitude) * zoom_coeff_ + padding_
    };
}

}//end namespace map_renderer
/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршрутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */
