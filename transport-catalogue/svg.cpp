#include "svg.h"

namespace svg {
using namespace std::literals;

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();
    // Делегируем вывод тега своим подклассам
    RenderObject(context);
    context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center){
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius){
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\""sv;
    RenderAttrs(out);
    out << "/>"sv;
}

//Polyline

Polyline& Polyline::AddPoint(Point point){
    points_.push_back(point);
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    std::ostream& out = context.out;
    out<<"<polyline points=\"";
    bool open= false;
    for (const Point poi:points_){
        if (open){
            out<<" ";
        }
        out<<poi.x<<","<<poi.y;
        open = true;     
    }
    out << "\"";
    RenderAttrs(out);
    out << "/>";
}

//Text

Text& Text::SetPosition(Point pos){
    pos_ = pos;
    return *this;
}

Text& Text::SetOffset(Point offset){
    offset_ = offset;
    return *this;
}

Text& Text::SetFontSize(uint32_t size){
    size_ = size;
    return *this;
}

Text& Text::SetFontFamily(std::string font_family){
    font_family_ = std::move(font_family);
    return *this;
}

Text& Text::SetFontWeight(std::string font_weight){
    font_weight_ = std::move(font_weight);
    return *this;
}

Text& Text::SetData(std::string data){
    data_ = std::move(data);
    return *this;
}

std::string Text::DeleteSpaces(const std::string& str) const {
    if (!str.empty()){
        auto left = str.find_first_not_of(' ');
        auto right = str.find_last_not_of(' ');
        return str.substr(left, right - left + 1);     
    }  
        return {};
}

 std::string Text::RetSymbols(const std::string& str) const {
    std::string new_str;
    for (char ch : str) {
        if (ch == '"') {
            new_str += "&quot;"sv;
            continue;    
        } else if (ch == '`' || ch == '\''){
            new_str += "&apos;"sv;
            continue;
        } else if (ch == '<'){
            new_str += "&lt;"sv;
            continue;  
        } else if (ch == '>'){
            new_str += "&gt;"sv;
            continue; 
        } else if (ch == '&'){
            new_str += "&amp;"sv;
            continue;  
        } else {}
        new_str += ch;
    }
    return new_str;
}

void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out<<"<text";
    RenderAttrs(out);
    out<<" x=\""<<pos_.x<<"\" y=\""<<pos_.y<<"\"";
    out<<" dx=\""<<offset_.x<<"\" dy=\""<<offset_.y<<"\"";
    out<<" font-size=\""<<size_<<"\"";
    if (font_family_.size()){
        out<<" font-family=\""<<font_family_<<"\"";
    }
    if (font_weight_.size()){
        out<<" font-weight=\""<<font_weight_<<"\"";
    }
    std::string text = RetSymbols(data_);
    std::string text1 = DeleteSpaces(text);
    out<<">"<<text1<<"<";
    out<<"/text>";

}

void Document::AddPtr(std::unique_ptr<Object> &&obj){
    objects_.emplace_back(std::move(obj));
}

void Document::Render(std::ostream& out ) const{
    RenderContext context(out, 2, 2);
    out<<R"(<?xml version="1.0" encoding="UTF-8" ?>)"<<"\n";
    out<<R"(<svg xmlns="http://www.w3.org/2000/svg" version="1.1">)"<<"\n";
    for (const auto& obj:objects_){
        obj->Render(context);
    }
    out << "</svg>";
}

}  // namespace svg

namespace shapes{

    Triangle::Triangle(svg::Point p1, svg::Point p2, svg::Point p3): p1_(p1), p2_(p2), p3_(p3)
    {
    }

    void Triangle::Draw(svg::ObjectContainer& container) const {
        container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
    }

    Star::Star(svg::Point center, double outer_rad, double inner_rad, int num_rays)
    :center_(center)
    ,outer_rad_(outer_rad)
    ,inner_rad_(inner_rad)
    ,num_rays_(num_rays)
    {
    }

    void Star::Draw(svg::ObjectContainer& container) const {
        svg::Polyline polyline;
        for (int i = 0; i <= num_rays_; ++i) {
            double angle = 2 * M_PI * (i % num_rays_) / num_rays_;
            polyline.AddPoint({center_.x + outer_rad_ * sin(angle), center_.y - outer_rad_ * cos(angle)});
            if (i == num_rays_) {
                break;
            }
            angle += M_PI / num_rays_;
            polyline.AddPoint({center_.x + inner_rad_ * sin(angle), center_.y - inner_rad_ * cos(angle)});
        }
        container.Add(polyline.SetFillColor(into).SetStrokeColor(out));
    }

    Snowman::Snowman(svg::Point center, double rad)
    :center_(center)
    ,rad_(rad)
    ,center1_({center.x,center.y+rad*2})
    ,rad1_(rad*1.5)
    ,center2_({center.x,center.y+rad*5})
    ,rad2_(rad*2)
    {
    }

    void Snowman::Draw(svg::ObjectContainer& container) const {
        container.Add(svg::Circle().SetCenter(center2_).SetRadius(rad2_).SetFillColor(into).SetStrokeColor(out));
        container.Add(svg::Circle().SetCenter(center1_).SetRadius(rad1_).SetFillColor(into).SetStrokeColor(out));
        container.Add(svg::Circle().SetCenter(center_).SetRadius(rad_).SetFillColor(into).SetStrokeColor(out));
    }
}