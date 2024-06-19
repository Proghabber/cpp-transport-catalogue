#pragma once
#include "transport_catalogue.h"
#include "map_renderer.h"
#include <optional>
#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>
#include <set>


namespace handler {

  class RequestHandler {
  public:
    RequestHandler(catalogue::TransportCatalogue& db, render::SvgMaker& map);
    void SaveRequest(data_handler::AllRequest&& requests); //заполнит транспорт и сохранит нзапросы на сохнанениен запросов
    void SaveAnswers(std::vector<data_handler::RetRequest>&& requests_vec); //сохранит ответы (понадобится для создания Json  или xml ответа)
    void SaveSvgOption(render::SvgOption&& requests_svg); //сохранит настройки изображения
    std::vector<data_handler::AllInfo> GetAnswers(); // создаст и вернет вектор ответов из транспорт
    void AddStop(const std::string& id, geo_math::Coordinates point);
    void AddStopsDistance(const std::string_view stop_one, const std::string_view stop_two, const int distance);
    void AddBus(const std::string& id, const std::vector<std::string_view> stops, bool is_roundtrip);
    data_bus::InfoBus GetBusStat(const std::string_view& bus_name) const;// Возвращает информацию о маршруте (запрос Bus)
    data_bus::InfoStop GetStopStat(const std::string_view& bus_name) const;// Возвращает информацию о маршруте (запрос stop)
    std::vector<geo_math::Coordinates> SavePoints(const data_bus::BusMap&  busses, const data_bus::StopMap& stops); //создает список кординат для вычесления точек на карте
    void MakeImage(std::ostream& out);
  private:
    catalogue::TransportCatalogue& db_;
    render::SvgMaker& renderer_;
    render::SvgOption svg_options_;
    std::vector<data_handler::RetRequest> out_requests; // все запросы на вывод из транспорт
    
};

std::vector<std::string_view> RetRefferense(std::vector<std::string>& text); //вспомогательная функция пересоздаст вектор остановок из bus_request в вектор ссылок на них
}


