#pragma once
#include <iostream>
#include <optional>
#include <algorithm>
#include <cstdlib>

#include "domain.h"
#include "geo.h"
#include "svg.h"



using namespace transport_catalogue;

namespace map_renderer {


const double EPSILON = 1e-6;



class SphereProjector {
public:
     SphereProjector() = default;
    template <typename InputIt>
    SphereProjector(InputIt points_begin,
                    InputIt points_end,
                    double max_width,
                    double max_height,
                    double padding);

    svg::Point operator()(transport_catalogue::detail::geo::Coordinates coords) const;

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;

    bool IsZero(double value) {
        return std::abs(value) < EPSILON;
    }

};


struct RenderSettings{
    std::pair<double, double> bus_label_offset; // dx dy смещение надписи с названием маршрута относительно координат конечной остановки на карте

    std::pair<double, double> stop_label_offset;  // dx dy смещение названия остановки относительно её координат на карте.

        double width,
        height,
        padding,
        line_width,
        underlayer_width,
        stop_radius;

        int bus_label_font_size,
        stop_label_font_size;

        std::vector<svg::Color> color_palette;
        svg::Color underlayer_color;

};

struct BusStopForMap{
  std::string name;
  detail::geo::Coordinates coordinates;
  bool is_roundtrip;
};

using BusRoutes = std::vector<std::pair<std::string, std::vector<BusStopForMap>>>;

class MapRenderer {
public:
    MapRenderer(RenderSettings&& render_settings, BusRoutes& br);





    void PrintDocument(std::ostream& out);


private:
    RenderSettings render_settings_;
    BusRoutes& bus_routes_;
    svg::Document document_map_;
    SphereProjector sphere_projector_;
    std::vector<BusStopForMap> bus_stops_all_;


    SphereProjector CreateSphereProjector();
    svg::Polyline CreateRouteline(const std::vector<BusStopForMap>& bus_route, svg::Color color);
    void CreateRouteTitle(const std::string& bus_name, const std::vector<BusStopForMap>& stops, svg::Color color);
    void CreateStopsSymbols();
    void CreateStopsTitles();
    void CreateBusStopsAll();
    void CreateRoutes();
};











template <typename InputIt>
    SphereProjector::SphereProjector(InputIt points_begin,
                                     InputIt points_end,
                                     double max_width,
                                     double max_height,
                                     double padding) : padding_(padding) {
        if (points_begin == points_end) {
            return;
        }

        const auto [left_it,
                    right_it] = std::minmax_element(points_begin,
                                                    points_end,
            [](auto lhs, auto rhs) {
                return lhs.longitude < rhs.longitude;
            });

        min_lon_ = left_it->longitude;
        const double max_lon = right_it->longitude;

        const auto [bottom_it,
                    top_it] = std::minmax_element(points_begin,
                                                  points_end,
            [](auto lhs, auto rhs) {
                return lhs.latitude < rhs.latitude;
            });

        const double min_lat = bottom_it->latitude;
        max_lat_ = top_it->latitude;

        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding)
                         / (max_lon - min_lon_);
        }

        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding)
                          / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            zoom_coeff_ = std::min(*width_zoom,
                                   *height_zoom);
        } else if (width_zoom) {
            zoom_coeff_ = *width_zoom;

        } else if (height_zoom) {
            zoom_coeff_ = *height_zoom;

        }
    }

}//end namespace map_renderer
/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршрутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */
