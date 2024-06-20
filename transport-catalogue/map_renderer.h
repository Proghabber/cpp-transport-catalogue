#pragma once
#include "geo.h"
#include "svg.h"
#include "domain.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>

namespace render{
    struct SvgOption{
        double width;
        double height;
        double padding;
        double stop_radius;
        double line_width;
        int bus_label_font_size;
        std::vector<double> bus_label_offset;
        int stop_label_font_size;
        std::vector<double> stop_label_offset;
        svg::Color underlayer_color;
        double underlayer_width;
        std::vector<svg::Color> color_palette;
    };

    inline const double EPSILON = 1e-6;
    bool IsZero(double value);

    using set_bus = std::map<std::string_view, std::vector<std::pair<std::string_view, geo_math::Coordinates>>>;
    using collect_bus = std::vector<set_bus>;

    class SphereProjector{
    public:
        // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
        template <typename PointInputIt>
        SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                        double max_width, double max_height, double padding)
            : padding_(padding) //
        {
            // Если точки поверхности сферы не заданы, вычислять нечего
            if (points_begin == points_end){
                return;
            }

            // Находим точки с минимальной и максимальной долготой
            const auto [left_it, right_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs)
                { return lhs.lng < rhs.lng; });
            min_lon_ = left_it->lng;
            const double max_lon = right_it->lng;

            // Находим точки с минимальной и максимальной широтой
            const auto [bottom_it, top_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs)
                { return lhs.lat < rhs.lat; });
            const double min_lat = bottom_it->lat;
            max_lat_ = top_it->lat;

            // Вычисляем коэффициент масштабирования вдоль координаты x
            std::optional<double> width_zoom;
            if (!IsZero(max_lon - min_lon_)){
                width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
            }

            // Вычисляем коэффициент масштабирования вдоль координаты y
            std::optional<double> height_zoom;
            if (!IsZero(max_lat_ - min_lat)){
                height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
            }

            if (width_zoom && height_zoom){
                // Коэффициенты масштабирования по ширине и высоте ненулевые,
                // берём минимальный из них
                zoom_coeff_ = std::min(*width_zoom, *height_zoom);
            }
            else if (width_zoom){
                // Коэффициент масштабирования по ширине ненулевой, используем его
                zoom_coeff_ = *width_zoom;
            }
            else if (height_zoom){
                // Коэффициент масштабирования по высоте ненулевой, используем его
                zoom_coeff_ = *height_zoom;
            }
        }

        // Проецирует широту и долготу в координаты внутри SVG-изображения
        svg::Point operator()(geo_math::Coordinates coords) const {
            return {
                (coords.lng - min_lon_) * zoom_coeff_ + padding_,
                (max_lat_ - coords.lat) * zoom_coeff_ + padding_};
        }
    private:
        double padding_;
        double min_lon_ = 0;
        double max_lat_ = 0;
        double zoom_coeff_ = 0;
    };

    
    class SvgMaker{
    public:
        void MakeImage(std::ostream &out, const data_bus::BusMap &busses, const data_bus::StopMap &stops, SvgOption &&options);
    private:    
        SvgOption svg_options_;
        std::unordered_map<std::string_view, size_t> bus_color_;                                      // имена маршрутов и цвета
        std::vector<geo_math::Coordinates> SavePoints(const data_bus::BusMap &busses, const data_bus::StopMap &stops); // создает список кординат для вычесления точек на карте

        std::vector<geo_math::Coordinates> ReferryStopPoins(const data_bus::BusMap &busses, const data_bus::StopMap &stops, const std::string_view bus); // соотнесет остановки и их кординаты
        void CreateLineBus(svg::Document &doc, const data_bus::BusMap &busses, const data_bus::StopMap &stops, SphereProjector &svg_cordinates);         // создаст линию маршрута
        void CreateTextBus(svg::Document &doc, const data_bus::BusMap &busses, const data_bus::StopMap &stops, SphereProjector &svg_cordinates);         //
        void CreateLabelStops(svg::Document &doc, const data_bus::BusMap &busses, const data_bus::StopMap &stops, SphereProjector &svg_cordinates);      //
        void CreateTextStop(svg::Document &doc, const data_bus::BusMap &busses, const data_bus::StopMap &stops, SphereProjector &svg_cordinates);        //

        svg::Polyline MakeLine(std::vector<geo_math::Coordinates> &&points, const SvgOption &options, const render::SphereProjector &svg_cordinates, size_t number_color);
        svg::Text MakeBus(const std::string_view name, const SvgOption &options, const geo_math::Coordinates &point, const render::SphereProjector &svg_cordinates, size_t number_color);
        svg::Text MakeBackBus(const std::string_view name, const SvgOption &options, const geo_math::Coordinates &point, const render::SphereProjector &svg_cordinates);
        svg::Circle MakeLabel(const SvgOption &options, const render::SphereProjector &svg_cordinates, const geo_math::Coordinates &point);
        svg::Text MakeStop(const std::string_view name, const SvgOption &options, const geo_math::Coordinates &point, const render::SphereProjector &svg_cordinates);
        svg::Text MakeBackStop(const std::string_view name, const SvgOption &options, const geo_math::Coordinates &point, const render::SphereProjector &svg_cordinates);
    };

}