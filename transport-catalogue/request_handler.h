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
        void MakeImage(std::ostream& out);
    private:
        catalogue::TransportCatalogue& db_;
        render::SvgMaker& renderer_;
        render::SvgOption svg_options_;
        std::vector<data_handler::RetRequest> out_requests; // все запросы на вывод из транспорт    
    };
}


