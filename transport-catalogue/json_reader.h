#pragma once

#include <map>
#include <vector>
#include <sstream> 
#include <iostream>
#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "request_handler.h"


namespace readJson{
    class JsonReader{
    private:
        data_handler::Stop_request ParseStation(json::Dict& dict); // вернет остановку и словарь становок с дистанцией до них
        data_handler::Bus_request ParseBus(json::Dict& dict); // вернет маршрут его остановки и информацию о закольцованности
        void ParseDistance(json::Dict& dist,data_handler::Stop_request& stop ); // заполнит словарь остановок и растояний для них вструктуре stop_request
        void ParseInput(json::Array& array, handler::RequestHandler& saver); //заполнит структуру запросов на сохранение
        void ParseOutput(json::Array& array, handler::RequestHandler& saver); // распарсит и сохранит список запросов на вывот из транспорт
        void ParseRenderSetting(json::Dict& dict, handler::RequestHandler& saver); // распарсит и сохранит настройки изображения маршрута
        svg::Color ToColor(json::Node& node); // вспомогательная функция опредилит в каком формате задан цвет в запросе и вернет в нужном формате
        void CreateJsonBus(std::vector<json::Node>& nodes, data_handler::AllInfo& data );
        void CreateJsonStop(std::vector<json::Node>& nodes, data_handler::AllInfo& data );
        void CreateJsonMap(std::vector<json::Node>& nodes, data_handler::AllInfo& data, handler::RequestHandler& saver );
    public:
        void ParseRequests(std::istream& input, handler::RequestHandler& saver ); //парсим все запросы и сохраняем в request_handler
        std::string Print(handler::RequestHandler& saver); //для опытов вывода в поток
        json::Node ReturnAnswer(handler::RequestHandler& saver); //создает json для вывода
    };
}



