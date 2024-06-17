#pragma once
#include "transport_catalogue.h"
#include "map_renderer.h"
#include <optional>
//#include <unordered_set>
#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>
#include <set>
#include <variant>



/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * В качестве источника для идей предлагаем взглянуть на нашу версию обработчика запросов.
 * Вы можете реализовать обработку запросов способом, который удобнее вам.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

// Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
// с другими подсистемами приложения.
// См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)
/*
class RequestHandler {
public:
    // MapRenderer понадобится в следующей части итогового проекта
    RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer);

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    const std::unordered_set<BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;

    // Этот метод будет нужен в следующей части итогового проекта
    svg::Document RenderMap() const;

private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const TransportCatalogue& db_;
    const renderer::MapRenderer& renderer_;
};
*/


    
   namespace handler {

    struct map_request{
      int id;
      bool answer;
    };

    struct bus_request{
        std::string name;
        bool is_roundtrip;
        std::vector<std::string> stops;
    };

    struct stop_request{
      std::string name;
      geo_math::Coordinates point;
      std::map<std::string, int> distance;

    };

    struct AllRequest{
      std::vector<bus_request> buses;
      std::vector<stop_request> stops;

    };

    struct RetRequest{
      int id;
      std::string type;
      std::string name;
    };

    struct BusCollect{
      std::vector<std::string_view> stops;
      std::vector<geo_math::Coordinates> cordinates;
      int color_number = 0;
    }; 

    using AllInfo = std::variant<std::monostate, std::pair<catalogue::InfoBus,int>, std::pair<catalogue::InfoStop,int>, map_request>;
    using BusMap = std::map<std::string_view, catalogue::Bus>;
    using StopMap = std::map<std::string_view, catalogue::Stop *> ;

    

    class RequestHandler {
    public:
      // MapRenderer понадобится в следующей части итогового проекта
      RequestHandler(catalogue::TransportCatalogue& db, render::SvgMaker& map);
      ///void Parsing(std::istream &input)

      void SaveRequest(handler::AllRequest&& requests); //заполнит транспорт и сохранит нзапросы на сохнанениен запросов
      void SaveAnswers(std::vector<handler::RetRequest>&& requests_vec); //сохранит ответы (понадобится для создания Json  или xml ответа)
      void SaveSvgOption(render::SvgOption&& requests_svg); //сохранит настройки изображения
      std::vector<handler::AllInfo> GetAnswers(); // создаст и вернет вектор ответов из транспорт
      void AddStop(const std::string& id, geo_math::Coordinates point);
      void AddStopsDistance(const std::string_view stop_one, const std::string_view stop_two, const int distance);
      void AddBus(const std::string& id, const std::vector<std::string_view> stops, bool is_roundtrip);
      catalogue::InfoBus GetBusStat(const std::string_view& bus_name) const;// Возвращает информацию о маршруте (запрос Bus)
      catalogue::InfoStop GetStopStat(const std::string_view& bus_name) const;// Возвращает информацию о маршруте (запрос stop)
      std::vector<geo_math::Coordinates> SavePoints(const BusMap&  busses, const StopMap& stops); //создает список кординат для вычесления точек на карте
      void MakeImage(std::ostream& out);

    
      

  private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    catalogue::TransportCatalogue& db_;
    render::SvgMaker& renderer_;
   
    std::vector<handler::RetRequest> out_requests; // все запросы на вывод из транспорт
    render::SvgOption svg_options; // настройки для изображения карты
    std::unordered_map<std::string_view, int> bus_color_; // имена маршрутов и цвета

    void CreateLineBus(const render::SphereProjector& svg_cordinates, svg::Document& doc, const BusMap& busses, const StopMap& stops);//создаст линию маршрута
    void CreateTextBus( const render::SphereProjector& svg_cordinates, svg::Document& doc, const BusMap& busses, const StopMap& stops);//
    void CreateLableStops(const render::SphereProjector& svg_cordinates, svg::Document& doc, const BusMap& busses, const StopMap& stops);//
    void CreateTextStop(const render::SphereProjector& svg_cordinates, svg::Document& doc, const BusMap& busses, const StopMap& stops);//
    std::vector<geo_math::Coordinates> ReferryStopPoins(const BusMap& busses, const StopMap& stops, const std::string_view bus);// соотнесет остановки и их кординаты
};

std::vector<std::string_view> RetRefferense(std::vector<std::string>& text); //вспомогательная функция пересоздаст вектор остановок из bus_request в вектор ссылок на них

   }


