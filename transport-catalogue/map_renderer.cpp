#include "map_renderer.h"

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршртутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */

namespace render{
    bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}



    svg::Polyline SvgMaker::MakeLine(std::vector<geo_math::Coordinates>&& points, const SvgOption &options, const render::SphereProjector& svg_cordinates, size_t number_color ){
        svg::Polyline line;

        for(const geo_math::Coordinates& point: points){
            line.AddPoint(svg_cordinates(point));
        }
        line.SetFillColor("none");//fill
        line.SetStrokeColor(options.color_palette.at(number_color));//stroke
        line.SetStrokeWidth(options.line_width);//stroke-width
        line.SetStrokeLineCap(StrokeLineCap::ROUND);//stroke-linecap
        line.SetStrokeLineJoin(StrokeLineJoin::ROUND);//stroke-linejoin

        return line;
    }

    svg::Text SvgMaker::MakeBus(const std::string_view name, const SvgOption &options, const geo_math::Coordinates& point, const render::SphereProjector& svg_cordinates, size_t number_color ){
        svg::Text text;

        text.SetPosition(svg_cordinates(point));// x,y
        text.SetOffset({options.bus_label_offset.at(0), options.bus_label_offset.at(1)});//dx,dy
        text.SetFontSize(options.bus_label_font_size);// font-size
        text.SetFontFamily("Verdana");//font-family
        text.SetFontWeight("bold");//font-weight
        text.SetData(std::string(name));// text
        text.SetFillColor(options.color_palette.at(number_color));//fill
       
        return text;
    }
    svg::Text SvgMaker::MakeBackBus(const std::string_view name, const SvgOption &options, const geo_math::Coordinates &point, const render::SphereProjector &svg_cordinates ){
        svg::Text text;

        text.SetPosition(svg_cordinates(point));// x,y
        text.SetOffset({options.bus_label_offset.at(0), options.bus_label_offset.at(1)});//dx,dy
        text.SetFontSize(options.bus_label_font_size);// font-size
        text.SetFontFamily("Verdana");//font-family
        text.SetFontWeight("bold");//font-weight
        text.SetData(std::string(name));// text
        text.SetFillColor(options.underlayer_color);//fill
        text.SetStrokeColor(options.underlayer_color); //stroke
        text.SetStrokeWidth(options.underlayer_width); //stroke-width
        text.SetStrokeLineCap(StrokeLineCap::ROUND); //stroke-linecap
        text.SetStrokeLineJoin(StrokeLineJoin::ROUND);//stroke-linejoin
        
        return text;
    }
    svg::Circle SvgMaker::MakeLable(const SvgOption& options, const render::SphereProjector &svg_cordinates, const geo_math::Coordinates& point){
        svg::Circle lable;

        lable.SetCenter(svg_cordinates(point));
        lable.SetRadius(options.stop_radius);
        lable.SetFillColor("white");

        return lable;
    }
    svg::Text SvgMaker::MakeStop(const std::string_view name, const SvgOption &options, const geo_math::Coordinates &point, const render::SphereProjector &svg_cordinates){
        svg::Text text;

        text.SetFillColor("black");//fill
        text.SetPosition(svg_cordinates(point));// x,y
        text.SetOffset({options.stop_label_offset.at(0), options.stop_label_offset.at(1)});//dx,dy
        text.SetFontSize(options.stop_label_font_size);// font-size
        text.SetFontFamily("Verdana");//font-family
        text.SetData(std::string(name));// text

        return text;

    }
    svg::Text SvgMaker::MakeBackStop(const std::string_view name, const SvgOption &options, const geo_math::Coordinates &point, const render::SphereProjector &svg_cordinates){
        svg::Text text;
        
        text.SetFillColor(options.underlayer_color);//fill
        text.SetStrokeColor(options.underlayer_color);//stroke
        text.SetStrokeWidth(options.underlayer_width); //stroke width
        text.SetStrokeLineCap(StrokeLineCap::ROUND); //stroke-linecap
        text.SetStrokeLineJoin(StrokeLineJoin::ROUND);//stroke-linejoin
        text.SetPosition(svg_cordinates(point));// x,y
        text.SetOffset({options.stop_label_offset.at(0), options.stop_label_offset.at(1)});//dx,dy
        text.SetFontSize(options.stop_label_font_size);// font-size
        text.SetFontFamily("Verdana");//font-family
        text.SetData(std::string(name));// text

        return text;
    }
}