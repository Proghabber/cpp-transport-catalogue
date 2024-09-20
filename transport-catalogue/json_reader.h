#pragma once

#include <map>
#include <vector>
#include <sstream> 
#include <iostream>
#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "transport_router.h"
#include "json_builder.h"

namespace readJson{
    class JsonReader{
    private:
        json::Array base_requests_;// базовые запросы
        json::Array stat_requests_;// запросы возврата ответа
        json::Dict render_setting_;// запросы настроек 
        json::Dict router_settings_;// запросы маршрута 


        data_handler::StopRequest ParseStation(json::Dict& dict); // вернет остановку и словарь становок с дистанцией до них
        data_handler::BusRequest ParseBus(json::Dict& dict); // вернет маршрут его остановки и информацию о закольцованности
        void ParseDistance(json::Dict& dist,data_handler::StopRequest& stop ); // заполнит словарь остановок и растояний для них вструктуре StopRequest
        svg::Color ToColor(json::Node& node); // вспомогательная функция опредилит в каком формате задан цвет в запросе и вернет в нужном формате
        void CreateJsonBus(std::vector<json::Node>& nodes,const data_handler::AllInfo& data );
        void CreateJsonStop(std::vector<json::Node>& nodes,const data_handler::AllInfo& data );
        void CreateJsonMap(std::vector<json::Node>& nodes,const data_handler::AllInfo& data);
        void CreateJsonRout(std::vector<json::Node>& nodes,const data_handler::AllInfo& data );
        json::Node ReturnAnswer(const std::vector<data_handler::AllInfo>& answers); //создает json для вывода

    public:
        void ParseRequests(std::istream& input); //парсим все запросы  
        void ParseInput(data_handler::AllRequest& in_requests); //заполнит структуру запросов на сохранение
        void ParseOutput(std::vector<data_handler::UniversalRequest>& out_requests); // распарсит и сохранит список запросов на вывод из транспорт
        void ParseRenderSetting(render::SvgOption& svg_options); // распарсит и сохранит настройки изображения маршрута
        void ParseRoutsSettings(transport_router::RouteParameters& router_parametrs); //распарсит и сохранит настройки маршрутов
        
        void Print(std::ostream& output, const std::vector<data_handler::AllInfo>& answers); // создаст json с ответами и вернет его впоток
    };
}



