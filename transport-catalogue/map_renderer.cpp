#include "map_renderer.h"

namespace render
{
    bool IsZero(double value){
        return std::abs(value) < EPSILON;
    }

    void SvgMaker::MakeImage(std::ostream &out, const data_bus::BusMap &busses, const data_bus::StopMap &stops, SvgOption &&options){
        svg_options_ = options;
        std::vector<geo_math::Coordinates> cordinates_all = (SavePoints(busses, stops));

        SphereProjector svg_cordinates = SphereProjector(cordinates_all.begin(), cordinates_all.end(),
                                                         svg_options_.width, 
                                                         svg_options_.height, 
                                                         svg_options_.padding);
        svg::Document doc;
        CreateLineBus(doc, busses, stops, svg_cordinates);

        // надписи маршрутов
        CreateTextBus(doc, busses, stops, svg_cordinates);

        // точки
        CreateLabelStops(doc, busses, stops, svg_cordinates);

        // названия остановок
        CreateTextStop(doc, busses, stops, svg_cordinates);

        // std::cout<<12;
        doc.Render(out);
    }

    std::vector<geo_math::Coordinates> SvgMaker::SavePoints(const data_bus::BusMap &busses, const data_bus::StopMap &stops){
        std::vector<geo_math::Coordinates> cordinates_all;
        std::set<std::string_view> stops_etalon;
        for (std::pair<std::string_view, data_bus::Bus> bus : busses){
            for (std::string_view stop : bus.second.stops){   
                if (!stops_etalon.count(stop)){
                    stops_etalon.insert(stop);
                    cordinates_all.push_back(stops.at(stop)->point);
                }
            }
        }
        return cordinates_all;
    }

    std::vector<geo_math::Coordinates> SvgMaker::ReferryStopPoins(const data_bus::BusMap &busses, const data_bus::StopMap &stops, const std::string_view bus)
    {
        std::vector<geo_math::Coordinates> points;
        std::vector<std::string_view> stops_name = busses.at(bus).MakeFullBus();
        for (std::string_view stop : stops_name){
            points.push_back(stops.at(stop)->point);
        }
        return points;
    }

    void SvgMaker::CreateLineBus(svg::Document &doc, const data_bus::BusMap &busses, const data_bus::StopMap &stops, SphereProjector &svg_cordinates){
        size_t number_color = 0;
        const size_t max_number_color = svg_options_.color_palette.size();
        for (std::pair<std::string_view, data_bus::Bus> bus : busses){
            if (number_color == max_number_color){
                number_color = 0;
            }
            bus_color_[bus.first] = number_color;
            doc.Add(MakeLine(std::move(ReferryStopPoins(busses, stops, bus.first)), svg_options_, svg_cordinates, bus_color_[bus.first]));
            number_color++;
        }
    }

    void SvgMaker::CreateTextBus(svg::Document &doc, const data_bus::BusMap &busses, const data_bus::StopMap &stops, SphereProjector &svg_cordinates){
        for (std::pair<std::string_view, data_bus::Bus> bus : busses){
            const size_t end_stop = bus.second.stops.size() - 1;
            std::vector<geo_math::Coordinates> points = ReferryStopPoins(busses, stops, bus.first);
            doc.Add(MakeBackBus(bus.first, svg_options_, points.at(0), svg_cordinates));
            doc.Add(MakeBus(bus.first, svg_options_, points.at(0), svg_cordinates, bus_color_.at(bus.first)));
            if (bus.second.is_roundtrip == false && *bus.second.stops.begin() != *(bus.second.stops.begin() + end_stop)){
                doc.Add(MakeBackBus(bus.first, svg_options_, points.at(end_stop), svg_cordinates));
                doc.Add(MakeBus(bus.first, svg_options_, points.at(end_stop), svg_cordinates, bus_color_.at(bus.first)));
            }
        }
    }

    void SvgMaker::CreateLabelStops(svg::Document &doc, const data_bus::BusMap &busses, const data_bus::StopMap &stops, SphereProjector &svg_cordinates){
        std::set<std::string_view> name_stops;
        for (std::pair<std::string_view, data_bus::Bus> bus : busses){
            name_stops.insert(bus.second.stops.begin(), bus.second.stops.end());
        }
        for (std::pair<std::string_view, data_bus::Stop *> stop : stops){
            if (name_stops.count(stop.first)){
                doc.Add(MakeLabel(svg_options_, svg_cordinates, stop.second->point));
            }
        }
    }

    void SvgMaker::CreateTextStop(svg::Document &doc, const data_bus::BusMap &busses, const data_bus::StopMap &stops, SphereProjector &svg_cordinates){
        std::set<std::string_view> name_stops;
        for (std::pair<std::string_view, data_bus::Bus> bus : busses){
            name_stops.insert(bus.second.stops.begin(), bus.second.stops.end());
        }
        for (std::pair<std::string_view, data_bus::Stop *> stop : stops){
            if (name_stops.count(stop.first)){
                doc.Add(MakeBackStop(stop.first, svg_options_, stop.second->point, svg_cordinates));
                doc.Add(MakeStop(stop.first, svg_options_, stop.second->point, svg_cordinates));
            }
        }
    }

    svg::Polyline SvgMaker::MakeLine(std::vector<geo_math::Coordinates> &&points, const SvgOption &options, const render::SphereProjector &svg_cordinates, size_t number_color){
        svg::Polyline line;
        for (const geo_math::Coordinates &point : points){
            line.AddPoint(svg_cordinates(point));
        }
        line.SetFillColor("none");                                   // fill
        line.SetStrokeColor(options.color_palette.at(number_color)); // stroke
        line.SetStrokeWidth(options.line_width);                     // stroke-width
        line.SetStrokeLineCap(StrokeLineCap::ROUND);                 // stroke-linecap
        line.SetStrokeLineJoin(StrokeLineJoin::ROUND);               // stroke-linejoin
        return line;
    }

    svg::Text SvgMaker::MakeBus(const std::string_view name, const SvgOption &options, const geo_math::Coordinates &point, const render::SphereProjector &svg_cordinates, size_t number_color){
        svg::Text text;

        text.SetPosition(svg_cordinates(point));                                          // x,y
        text.SetOffset({options.bus_label_offset.at(0), options.bus_label_offset.at(1)}); // dx,dy
        text.SetFontSize(options.bus_label_font_size);                                    // font-size
        text.SetFontFamily("Verdana");                                                    // font-family
        text.SetFontWeight("bold");                                                       // font-weight
        text.SetData(std::string(name));                                                  // text
        text.SetFillColor(options.color_palette.at(number_color));                        // fill
        return text;
    }

    svg::Text SvgMaker::MakeBackBus(const std::string_view name, const SvgOption &options, const geo_math::Coordinates &point, const render::SphereProjector &svg_cordinates){
        svg::Text text;

        text.SetPosition(svg_cordinates(point));                                          // x,y
        text.SetOffset({options.bus_label_offset.at(0), options.bus_label_offset.at(1)}); // dx,dy
        text.SetFontSize(options.bus_label_font_size);                                    // font-size
        text.SetFontFamily("Verdana");                                                    // font-family
        text.SetFontWeight("bold");                                                       // font-weight
        text.SetData(std::string(name));                                                  // text
        text.SetFillColor(options.underlayer_color);                                      // fill
        text.SetStrokeColor(options.underlayer_color);                                    // stroke
        text.SetStrokeWidth(options.underlayer_width);                                    // stroke-width
        text.SetStrokeLineCap(StrokeLineCap::ROUND);                                      // stroke-linecap
        text.SetStrokeLineJoin(StrokeLineJoin::ROUND);                                    // stroke-linejoin
        return text;
    }
    svg::Circle SvgMaker::MakeLabel(const SvgOption &options, const render::SphereProjector &svg_cordinates, const geo_math::Coordinates &point){
        svg::Circle lable;

        lable.SetCenter(svg_cordinates(point));
        lable.SetRadius(options.stop_radius);
        lable.SetFillColor("white");
        return lable;
    }
    svg::Text SvgMaker::MakeStop(const std::string_view name, const SvgOption &options, const geo_math::Coordinates &point, const render::SphereProjector &svg_cordinates){
        svg::Text text;

        text.SetFillColor("black");                                                         // fill
        text.SetPosition(svg_cordinates(point));                                            // x,y
        text.SetOffset({options.stop_label_offset.at(0), options.stop_label_offset.at(1)}); // dx,dy
        text.SetFontSize(options.stop_label_font_size);                                     // font-size
        text.SetFontFamily("Verdana");                                                      // font-family
        text.SetData(std::string(name));                                                    // text
        return text;
    }
    svg::Text SvgMaker::MakeBackStop(const std::string_view name, const SvgOption &options, const geo_math::Coordinates &point, const render::SphereProjector &svg_cordinates){
        svg::Text text;

        text.SetFillColor(options.underlayer_color);                                        // fill
        text.SetStrokeColor(options.underlayer_color);                                      // stroke
        text.SetStrokeWidth(options.underlayer_width);                                      // stroke width
        text.SetStrokeLineCap(StrokeLineCap::ROUND);                                        // stroke-linecap
        text.SetStrokeLineJoin(StrokeLineJoin::ROUND);                                      // stroke-linejoin
        text.SetPosition(svg_cordinates(point));                                            // x,y
        text.SetOffset({options.stop_label_offset.at(0), options.stop_label_offset.at(1)}); // dx,dy
        text.SetFontSize(options.stop_label_font_size);                                     // font-size
        text.SetFontFamily("Verdana");                                                      // font-family
        text.SetData(std::string(name));                                                    // text
        return text;
    }
}