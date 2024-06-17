#pragma once

#define _USE_MATH_DEFINES

#include <cmath>

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <sstream>

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

inline std::ostream& operator<<(std::ostream& out, const StrokeLineCap& strok) {
    if (strok == StrokeLineCap::BUTT){
        out<<"butt";
    }else if(strok == StrokeLineCap::ROUND){
        out<<"round";
    }else if(strok == StrokeLineCap::SQUARE){
        out<<"square";
    }
    return out;
}

inline std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& strok) {
    if (strok == StrokeLineJoin::ARCS){
        out<<"arcs";
    }else if(strok == StrokeLineJoin::BEVEL){
        out<<"bevel";
    }else if(strok == StrokeLineJoin::MITER){
        out<<"miter";
    }else if(strok == StrokeLineJoin::MITER_CLIP){
        out<<"miter-clip";
    }else if(strok == StrokeLineJoin::ROUND){
        out<<"round";
    }
    return out;
}




namespace svg {




//inline const Color NoneColor{"none"};

struct Rgb {
    Rgb() = default;
    Rgb(uint8_t red_, uint8_t green_, uint8_t blue_) 
    : red(red_)
    , green(green_)
    , blue(blue_){};

    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;

};

inline std::ostream& operator<<(std::ostream& out, const Rgb& color) {
    out<<static_cast<int>(color.red)<<","<<static_cast<int>(color.green)<<","<<static_cast<int>(color.blue);
    return out;
}

struct Rgba {
    Rgba() = default;
    Rgba(uint8_t red_, uint8_t green_, uint8_t blue_, double opacity_ ) 
    : red(red_)
    , green(green_)
    , blue(blue_)
    ,opacity(opacity_)
    {};
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    double opacity = 1.0;

};

inline std::ostream& operator<<(std::ostream& out, const Rgba& color) {
    out<<static_cast<int>(color.red)<<","<<static_cast<int>(color.green)<<","<<static_cast<int>(color.blue)<<","<<color.opacity;
    return out;
}


using Color = std::variant<std::monostate,std::string,Rgb,Rgba>;
inline const Color NoneColor{"none"}; 

 

struct ColorReturn {
    ColorReturn(std::ostringstream& strm): out(strm) {}
    std::ostream& out;
    void operator()(std::monostate) const {
        out<<"none";
    }
    void operator()(std::string color) const {     
        out<< color;
    }
    void operator()(Rgb color) const {
        out<<"rgb("<<color<<")";
    }

    void operator()(Rgba color) const {
        out<<"rgba("<<color<<")";
   
    }
};

inline std::ostream& operator<<(std::ostream& out, const Color& color) {
    std::ostringstream strm;
    std::visit(ColorReturn(strm),color);
    out<<strm.str();
    return out;
}













struct Point {
    Point() = default;
    Point(double x_, double y_)
        : x(x_)
        , y(y_) {
    }
    double x = 0;
    double y = 0;
};

/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
struct RenderContext {
    RenderContext(std::ostream& out_)
        : out(out_) {
    }

    RenderContext(std::ostream& out_, int indent_step_, int indent_ = 0)
        : out(out_)
        , indent_step(indent_step_)
        , indent(indent_) {
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

template <typename Owner>
class PathProps {
public:
    //virtual ~PathProps() = default;
    Owner& SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }

    Owner& SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }

    Owner& SetStrokeWidth(double width){
        width_=width;
        return AsOwner();
    }

    Owner& SetStrokeLineCap(StrokeLineCap line_cap){
        line_cap_=line_cap;
        return AsOwner();
    }

    Owner& SetStrokeLineJoin(StrokeLineJoin line_join){
        line_join_=line_join;
        return AsOwner();
    }

protected:
    ~PathProps() = default;

    // Метод RenderAttrs выводит в поток общие для всех путей атрибуты fill и stroke
    void RenderAttrs(std::ostream& out) const {
   
        if (fill_color_) {
            out <<" fill=\"" << *fill_color_ <<"\"";
        }
        if (stroke_color_) {
            out <<" stroke=\"" << *stroke_color_ << "\"";
        }
        if (width_) {
            out <<" stroke-width=\"" << *width_ << "\"";
        }
        if (line_cap_) {
            //StrokeLineCap cap= *line_cap_;
            out <<" stroke-linecap=\"" << *line_cap_ << "\"";
        }
        if (line_join_) {
            //StrokeLineJoin join=*line_join_;
            out <<" stroke-linejoin=\"" << *line_join_ << "\"";
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
    std::optional<double> width_;
    std::optional<StrokeLineCap> line_cap_;
    std::optional<StrokeLineJoin> line_join_;
};
    


/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */


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
    Point center_;
    double radius_ = 1.0;
    void RenderObject(const RenderContext& context) const override;

   
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
    std::vector<Point> points_;
    void RenderObject(const RenderContext& context) const override;
};

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text final : public Object, public PathProps<Text> {
public:
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

    // Прочие данные и методы, необходимые для реализации элемента <text>
private:

    
    Point pos_;
    Point offset_;
    uint32_t size_=1;
    std::string font_family_;
    std::string font_weight_;
    std::string data_;

    std::string DeleteSpaces(const std::string& str) const;
    std::string RetSymbols(const std::string& str)  const;
    void RenderObject(const RenderContext& context) const override;

};

class ObjectContainer{
public:

    virtual~ ObjectContainer()= default;
    virtual void AddPtr(std::unique_ptr<Object>&& obj)=0;

    template <typename ObjectType>
    void Add(ObjectType object) {
        AddPtr(std::make_unique<ObjectType>(std::move(object)));
    }

};

class Document: public ObjectContainer {
public:
   
    void AddPtr(std::unique_ptr<Object>&& obj) override;

    void Render(std::ostream& out) const;

private:
    int indent_step = 0;
    int indent = 0;
    std::vector<std::unique_ptr<Object>> objects_;

    
};


class Drawable {
public:
    virtual ~Drawable()=default;
    virtual void Draw(ObjectContainer& container)const =0;


};


}  // namespace svg

namespace shapes{


class Triangle:public svg::Drawable{
public:
    Triangle(svg::Point p1, svg::Point p2, svg::Point p3);
    void Draw(svg::ObjectContainer& container)const  override;

private:
    svg::Point p1_;
    svg::Point p2_; 
    svg::Point p3_;

};

class Star:public svg::Drawable{
public:
    Star(svg::Point center, double outer_rad, double inner_rad, int num_rays);
    void Draw(svg::ObjectContainer& container)const  override;

private:
    svg::Color into="red";
    svg::Color out="black";
    svg::Point center_;
    double outer_rad_; 
    double inner_rad_;
    int num_rays_;

};

class Snowman:public svg::Drawable{
public:
    Snowman(svg::Point center, double rad);
    void Draw(svg::ObjectContainer& container)const  override;

private:
    svg::Color into="rgb(240,240,240)";
    svg::Color out="black";
    svg::Point center_;
    double rad_=1;
    svg::Point center1_;
    double rad1_=1;
    svg::Point center2_;
    double rad2_=1;


};

}




