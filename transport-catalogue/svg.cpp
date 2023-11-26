#include "svg.h"

namespace svg {

using namespace std::literals;

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();
    // Делегируем вывод тега своим подклассам
    RenderObject(context);
    context.out << std::endl;
}

//-------------Color-------------------------------

/*
void ColorPrinter::operator()( Rgb& rgb) const{
    out << "rgb("sv << static_cast<short>(rgb.red_) << ","sv
                    << static_cast<short>(rgb.green_) << ","sv
                    << static_cast<short>(rgb.blue_) << ")"sv;
}
void ColorPrinter::operator()(Rgba& rgba) const{
    out << "rgba("sv << static_cast<short>(rgba.red_) << ","sv
                     << static_cast<short>(rgba.green_) << ","sv
                     << static_cast<short>(rgba.blue_) << ","sv
                     << (rgba.opacity_) << ")"sv;
}
void ColorPrinter::operator()(std::monostate) const{
    out << "none"sv;
}
void ColorPrinter::operator()( std::string& color) const{
    out << color;
}

std::ostream& operator<<(std::ostream& out, const Color& color){
    const auto col = color;
    std::visit(ColorPrinter{out}, col);
    return out;
}
*/


inline void PrintColor(std::ostream& out, Rgba& rgba){
    //out<<"rgba("<<rgba.red_<<","<<rgba.green_<<","<<rgba.blue_<<","<<rgba.opacity_<<")";
    out << "rgba("sv << static_cast<short>(rgba.red_) << ","sv
                     << static_cast<short>(rgba.green_) << ","sv
                     << static_cast<short>(rgba.blue_) << ","sv
                     << (rgba.opacity_) << ")"sv;
}
inline void PrintColor(std::ostream& out, Rgb& rgb){
    //out<<"rgb("<<rgb.red_<<","<<rgb.green_<<","<<rgb.blue_<<")";
    out << "rgb("sv << static_cast<short>(rgb.red_) << ","sv
                    << static_cast<short>(rgb.green_) << ","sv
                    << static_cast<short>(rgb.blue_) << ")"sv;
}
inline void PrintColor(std::ostream& out, std::monostate) {
    out << "none"sv;
}

inline void PrintColor(std::ostream& out, std::string& color) {
    out << color;
}
std::ostream& operator<<(std::ostream& out, const Color& color){
    std::visit([&out](auto value) {
            PrintColor(out, value);
    }, color);

    return out;
}


// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\""sv;
    RenderAttrs(out);
    out << " />"sv;
}

/* --------------------Polyline---------------------------------------
 *
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */


Polyline& Polyline::AddPoint(Point point){
    points_.emplace_back(point);
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    std::ostream& out = context.out;
    out<<R"(<polyline points=")";
    for(size_t i=0; i<points_.size(); i++){
           out << points_[i].x<< ","sv << points_[i].y;
        if (i != points_.size()-1){
            out << " ";
        }
    }
    out<<"\"";
    RenderAttrs(out);
    out<<" />"sv;

}

// Задаёт координаты опорной точки (атрибуты x и y)
Text& Text::SetPosition(Point pos){
    xy_ = pos;
    return *this;
}

// Задаёт смещение относительно опорной точки (атрибуты dx, dy)
Text& Text::SetOffset(Point offset){
    dxdy_= offset;
    return *this;
}

// Задаёт размеры шрифта (атрибут font-size)
Text& Text::SetFontSize(uint32_t size){
    font_size_ = size;
    return *this;
}

// Задаёт название шрифта (атрибут font-family)
Text& Text::SetFontFamily(std::string font_family){
    font_family_ = font_family;
    return *this;
}

// Задаёт толщину шрифта (атрибут font-weight)
Text& Text::SetFontWeight(std::string font_weight){
    font_weight_ = font_weight;
    return *this;
}

// Задаёт текстовое содержимое объекта (отображается внутри тега text)
Text& Text::SetData(std::string data){
    data_ = data;
    int i = 0;
    for (char sym:data){

        switch (sym) {
            case '"' :
                data_svg_ += "&quot";
                break;
            case '\'' :
                data_svg_ += "&apos";
                break;
            case '<' :
                data_svg_ += "&lt";
                break;
            case '>' :
                data_svg_ += "&gt";
                break;
            case '&':
                data_svg_ += "&amp";
                break;

            default : data_svg_ += sym;
        }

        i++;
    }
    return *this;
}

void Text::RenderObject(const RenderContext& context)  const  {
    std::ostream& out = context.out;

    out<<"<text";
            RenderAttrs(out);
            out<<" x=\"" <<xy_.x
            << "\" y=\""<< xy_.y <<"\""
            <<" dx=\"" << dxdy_.x
            << "\" dy=\""<< dxdy_.y <<"\""
            <<" font-size=\"" << font_size_<<"\"";
    if(font_family_ != ""){
        out << " font-family=\"" << font_family_ << "\"";
    }
    if(font_weight_ != ""){
        out << " font-weight=\"" << font_weight_ << "\"";
    }



    out<<">";
    out<< data_svg_ << "</text>";
    }
void Document::AddPtr(std::unique_ptr<Object>&& obj){
    objects_.emplace_back(move(obj));
}

void Document::Render(std::ostream& out) const{
    RenderContext context(out);
    out<<"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
    out<<"<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
    for(const auto& obj:objects_){
        obj->Render(out);
    }
    out<<"</svg>"<<std::endl;
}
/*
std::ostream& RgbOut(std::ostream& os, Color col){

    std::string_view sv = col;
        while(true){
           int pos sv
        }

    return os;
}
*/
}  // namespace svg
