#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <list>
#include <optional>
#include <ostream>
#include <variant>
#include <geo.h>


namespace svg {

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};

inline std::ostream& operator<<(std::ostream& out, StrokeLineCap slc){
    using namespace std::literals;
    switch (slc) {
        case StrokeLineCap::BUTT : out<<"butt"sv; break;
        case StrokeLineCap::ROUND : out<<"round"sv; break;
        case StrokeLineCap::SQUARE :   out<<"square"sv; break;
    }
    return out;
}

inline std::ostream& operator<<(std::ostream& out, StrokeLineJoin slj){
    using namespace std::literals;
    switch (slj) {
        case StrokeLineJoin::ARCS : out<<"arcs"sv ; break;
        case StrokeLineJoin::BEVEL : out<<"bevel"sv; break;
        case StrokeLineJoin::MITER :   out<<"miter"sv; break;
        case StrokeLineJoin::MITER_CLIP :   out<<"miter-clip"sv; break;
        case StrokeLineJoin::ROUND :   out<<"round"sv; break;
    }
    return out;
}

struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};

struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

//-------------Color-------------------------------
class Rgb{
  public:
    Rgb()=default;
    Rgb(uint8_t red, uint8_t green, uint8_t blue) : red_(red), green_(green), blue_(blue){}

    uint8_t red_=0;
    uint8_t green_=0;
    uint8_t blue_=0;
};


class Rgba{
  public:
    Rgba()=default;
    Rgba(uint8_t red, uint8_t green, uint8_t blue, double opacity ) : red_(red), green_(green), blue_(blue), opacity_(opacity){}

    uint8_t red_=0;
    uint8_t green_=0;
    uint8_t blue_=0;
    double opacity_ = 1.0;
};

using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;
inline const Color NoneColor{"none"};
inline void PrintColor(std::ostream& out, Rgb& rgb);
inline void PrintColor(std::ostream& out, Rgba& rgba);
inline void PrintColor(std::ostream& out, std::monostate);
inline void PrintColor(std::ostream& out, std::string& color);
std::ostream& operator<<(std::ostream& out, const Color& color);


template <typename Owner>
class PathProps{
  public:
    Owner& SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }

    Owner& SetStrokeLineCap(StrokeLineCap color) {
        SLC_ = std::move(color);
        return AsOwner();
    }

    Owner& SetStrokeLineJoin(StrokeLineJoin slj) {
        SLJ_ = std::move(slj);
        return AsOwner();
    }

    Owner& SetStrokeWidth(double width) {
        width_line_ = std::move(width);
        return AsOwner();
    }




protected:
    ~PathProps() = default;

    void RenderAttrs(std::ostream& out) const {
        using namespace std::literals;

        if (fill_color_) {
            out << " fill=\""sv << *fill_color_ << "\""sv;
        }
        if (stroke_color_) {
            out << " stroke=\""sv << *stroke_color_ << "\""sv;
        }

        if(width_line_){
            out << " stroke-width=\""sv << *width_line_ << "\""sv;
        }

        if(SLC_ != std::nullopt){
            out << " stroke-linecap=\""sv << SLC_.value() << "\""sv;
        }

        if(SLJ_ != std::nullopt){
            out << " stroke-linejoin=\""sv << SLJ_.value() << "\""sv;
        }


    }
private:

    Owner& AsOwner() {
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> width_line_;
    std::optional<StrokeLineCap> SLC_;
    std::optional<StrokeLineJoin> SLJ_;
};

class Object {
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override;

    Point center_;
    double radius_ = 1;
};

class Polyline final : public Object, public PathProps<Polyline> {
public:
    Polyline& AddPoint(Point point);
private:
    void RenderObject(const RenderContext& context) const override;
    std::vector<Point> points_;
};

class Text final : public Object, public PathProps<Text>{
public:
    Text() = default;
    Text& SetPosition(Point pos);
    Text& SetOffset(Point offset);
    Text& SetFontSize(uint32_t size);
    Text& SetFontFamily(std::string font_family);
    Text& SetFontWeight(std::string font_weight);
    Text& SetData(std::string data);
    void CreateText();
    void RenderObject(const RenderContext& context) const override;
private:
    Point xy_ = {0,0};
    Point dxdy_= {0,0};
    uint32_t font_size_ = 1;
    std::string font_family_="";
    std::string font_weight_ ="";
    std::string data_ ="";
    std::string data_svg_ ="";

};

class ObjectContainer{
    public:
    virtual ~ObjectContainer() = default;
    template<typename Obj>
    void Add(Obj obj) {
        objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
    }
    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;

protected:
    std::vector<std::unique_ptr<Object>> objects_;
};

class Drawable{
public:
    virtual ~Drawable() = default;
    virtual void Draw(ObjectContainer& oc) const = 0;

};

class Document : public ObjectContainer{
public:
    void AddPtr(std::unique_ptr<Object>&& obj);
    void Render(std::ostream& out) const;
};


}  // namespace svg
