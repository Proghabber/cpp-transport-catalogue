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
        //handler::AllRequest in_requests; // все запросы для сохранения в транспорт
        //std::vector<handler::RetRequest> out_requests; // все запросы на вывод из транспорт
        //render::SvgOption svg_options; // настройки для изображения карты

        handler::stop_request ParsStation(json::Dict& dict); // вернет остановку и словарь становок с дистанцией до них
        handler::bus_request ParsBus(json::Dict& dict); // вернет маршрут его остановки и информацию о закольцованности
        void ParsDistance(json::Dict& dist,handler::stop_request& stop ); // заполнит словарь остановок и растояний для них вструктуре stop_request
        void ParsInput(json::Array& array, handler::RequestHandler& saver); //заполнит структуру запросов на сохранение
        
        
       
        void ParsOutput(json::Array& array, handler::RequestHandler& saver); // распарсит и сохранит список запросов на вывот из транспорт
        void ParsRenderSetting(json::Dict& dict, handler::RequestHandler& saver); // распарсит и сохранит настройки изображения маршрута
        svg::Color DetectedColor(json::Node& nod); // вспомогательная функция опредилит в каком формате задан цвет в запросе и вернет в нужном формате
        void CreateJsonBus(std::vector<json::Node>& collect_nod, handler::AllInfo& data );
        void CreateJsonStop(std::vector<json::Node>& collect_nod, handler::AllInfo& data );
        void CreateJsonMap(std::vector<json::Node>& collect_nods, handler::AllInfo& data, handler::RequestHandler& saver );
      
    public:
        void ParsingRequests(std::istream& input, handler::RequestHandler& saver ); //парсим все запросы и сохраняем в request_handler
        std::string Print(handler::RequestHandler& saver); //для опытов вывода в поток
        json::Node ReturenAnswer(handler::RequestHandler& saver); //создает json для вывода
    };

    
   

    
    
}



