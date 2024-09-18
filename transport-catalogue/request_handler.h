#pragma once
#include "transport_catalogue.h"
#include "map_renderer.h"
//#include "transport_router.h"
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
        void FullTransport(data_handler::AllRequest&& requests); //заполнит транспорт 
        void SaveAnswers(std::vector<data_handler::RetRequest>&& requests_vec); //сохранит ответы (понадобится для создания Json  или xml ответа)
        void SaveSvgOption(render::SvgOption&& requests_svg); //сохранит настройки изображения
        std::vector<data_handler::AllInfo> GetAnswers(); // создаст и вернет вектор ответов из транспорт
        void MakeImage(std::ostream& out);
        //
        void SaveRoutSettings(std::map<std::string, double>&& set_rout);
        const std::map<std::string, double> GetRoutSettings() const;
        const data_bus::CollectStops& GetAllStops() const;
        const data_bus::CollectBus& GetAllBus() const;
        double GetStopsDist(std::string_view from, std::string_view to) const;
        void SavePath(data_handler::RoutAnswer path, int id);
        std::vector<data_handler::RetRequest>& RetRequests();
        

    private:
        catalogue::TransportCatalogue& db_;
        render::SvgMaker& renderer_;
        render::SvgOption svg_options_;
        std::map<std::string, double> rout_settings_;
        std::map<int, data_handler::RoutAnswer> rout_answers_; // 
        std::vector<data_handler::AllInfo> collect_answer_;// ответы на запросы
        std::vector<data_handler::RetRequest> out_requests_; // все запросы на вывод из транспорт 
         
        
    };
}


