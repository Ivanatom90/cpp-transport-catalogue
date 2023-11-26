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
#include "geo.h"


namespace svg {



// Объявив в заголовочном файле константу со спецификатором inline,
// мы сделаем так, что она будет одной на все единицы трансляции,
// которые подключают этот заголовок.
// В противном случае каждая единица трансляции будет использовать свою копию этой константы
//inline const Color NoneColor{"none"};

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

/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
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

/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */




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
/*
struct ColorPrinter{
  public:
   // ColorPrinter() = default;
   std::ostream& out;
   void operator()( Rgb& rgb) const;
   void operator()( Rgba& rgba) const;
   void operator()( std::monostate) const;
   void operator()( std::string& color) const;


};

std::ostream& operator<<(std::ostream& out, const Color& color);
*/

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
        // static_cast безопасно преобразует *this к Owner&,
        // если класс Owner — наследник PathProps
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> width_line_;
    std::optional<StrokeLineCap> SLC_;
    std::optional<StrokeLineJoin> SLJ_;
};

//std::ostream& RgbOut(Color col);

class Object {
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override;

    Point center_;
    double radius_ = 1;
};

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
class Polyline final : public Object, public PathProps<Polyline> {
public:
    // Добавляет очередную вершину к ломаной линии
    Polyline& AddPoint(Point point);

    /*
     * Прочие методы и данные, необходимые для реализации элемента <polyline>
     */
private:
    void RenderObject(const RenderContext& context) const override;
    std::vector<Point> points_;
};

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text final : public Object, public PathProps<Text>{
public:
    Text() = default;
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos);

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffset(Point offset);

    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(uint32_t size);

    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(std::string font_family);

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(std::string font_weight);

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& SetData(std::string data);

    void CreateText();

    void RenderObject(const RenderContext& context) const override;

    // Прочие данные и методы, необходимые для реализации элемента <text>
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
    /*
     Метод Add добавляет в svg-документ любой объект-наследник svg::Object.
     Пример использования:
     Document doc;
     doc.Add(Circle().SetCenter({20, 30}).SetRadius(15));
    */
    // void Add(???);

    // Добавляет в svg-документ объект-наследник svg::Object

    void AddPtr(std::unique_ptr<Object>&& obj);

    // Выводит в ostream svg-представление документа
    void Render(std::ostream& out) const;

    // Прочие методы и данные, необходимые для реализации класса Document
};


}  // namespace svg
