#include "map_renderer.h"

namespace map_renderer {

bool SphereProjector::IsZero(double value) {
    return std::abs(value) < EPSILON;
}

MapRenderer::MapRenderer(RenderSettings& render_settings) : render_settings_(render_settings) {}

svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
    return {(coords.longitude - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.latitude) * zoom_coeff_ + padding_};
}

SphereProjector MapRenderer::get_sphere_projector(const std::vector<geo::Coordinates>& points) const {
        return SphereProjector(points.begin(),
                               points.end(),
                               render_settings_.width_,
                               render_settings_.height_,
                               render_settings_.padding_);
}

RenderSettings MapRenderer::get_render_settings() const{
    return render_settings_;
}

int MapRenderer::get_palette_size() const{
    return render_settings_.color_palette_.size();
}

svg::Color MapRenderer::get_color(int line_number) const {
    return render_settings_.color_palette_[line_number];
}

std::vector<svg::Color> MapRenderer::get_color_palette() const{
    return render_settings_.color_palette_;
}

void MapRenderer::set_line_properties(svg::Polyline& polyline, [[maybe_unused]] int line_number) const {
    using namespace std::literals;

    polyline.SetStrokeColor(get_color(line_number));
    polyline.SetFillColor("none"s);
    polyline.SetStrokeWidth(render_settings_.line_width_);
    polyline.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    polyline.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
}

}
