#include "request_handler.h"

namespace handler {
    RequestHandler::RequestHandler(catalogue::TransportCatalogue &db, render::SvgMaker &map, readJson::JsonReader& json):db_(db), renderer_(map), reader_(json), router_(db_, router_settings_)
    {
    }

    void RequestHandler::ReadJson(std::istream& input){
        reader_.ParseRequests(input); //читаем 
        reader_.ParseInput(in_transport_);// сохраняем запросы для транспорта
        reader_.ParseOutput(out_requests_);// сохраняем запросы на вывод для транспорта , изображения и маршрутов
        reader_.ParseRenderSetting(svg_options_);// сохраняем настройки изображения
        reader_.ParseRoutsSettings(router_settings_);// сохраняем настройки маршрутов
    }

    void RequestHandler::ReturnJson(std::ostream& output){
        FullTransport();
        GetResponses();
        reader_.Print(output, collect_answer_);
    }

    void RequestHandler::FullTransport(){
        for (data_handler::StopRequest& stop: in_transport_.stops){
            db_.AddStop(stop.name,stop.point);  
            for (std::pair<const std::string, int> dist: stop.distance){
                db_.AddStopsDistance(stop.name, dist.first, dist.second);
            }
        }
        for (data_handler::BusRequest& bus: in_transport_.buses){
            std::vector<std::string_view> result;
            result.reserve(bus.stops.size());
            auto fun = [](const std::string& stop) -> std::string_view {return stop;};
            std::transform(bus.stops.cbegin(), bus.stops.cend(), std::back_inserter(result), fun);
            db_.AddBus(bus.name, result, bus.is_roundtrip);
        }
    }
    
    std::pair<data_bus::InfoStop,int> RequestHandler::GetStopResponse(const data_handler::UniversalRequest& request){
        int id = request.id; 
        data_bus::InfoStop stop(db_.FindStop(request.request_name)); 
        std::pair<data_bus::InfoStop,int> object; 
        object.first = stop; 
        object.second = id; 
        return object;
    }

    std::pair<data_bus::InfoBus,int> RequestHandler::GetBusResponse(const data_handler::UniversalRequest& request){
        int id = request.id; 
        data_bus::InfoBus bus(db_.FindBus(request.request_name)); 
        std::pair<data_bus::InfoBus,int> object; 
        object.first = bus; 
        object.second = id; 
        return object;
    }

    data_handler::MapRequest RequestHandler::GetMapResponse(const data_handler::UniversalRequest& request){
        data_handler::MapRequest object; 
        object.answer = true; 
        object.id = request.id; 
        object.svg = MakeImage();
        return object; 
    }

    data_handler::RouteSearchResponse RequestHandler::GetRoutResponse(const data_handler::UniversalRequest& request){
        return router_.FindPath(request);
    }

    void RequestHandler::GetResponses(){
        for (const data_handler::UniversalRequest& req: out_requests_){ 
            if (req.request_type == "Stop"){ 
                collect_answer_.push_back(GetStopResponse(req));
            } else if (req.request_type == "Bus"){ 
                collect_answer_.push_back(GetBusResponse(req));    
            } else if (req.request_type == "Map"){ 
                collect_answer_.push_back(GetMapResponse(req));
            } else if (req.request_type == "Route"){
                collect_answer_.push_back(GetRoutResponse(req));
            }
        } 
    }

    std::ostringstream RequestHandler::MakeImage(){
        data_bus::BusMap busses;
        data_bus::StopMap stops;
        std::ostringstream svg;
        for (const auto& entry : db_.GetAllBus()) {
            busses.emplace(entry.first, entry.second);
        }
        for (const auto& entry :db_.GetAllstops()) {
            stops.emplace(entry.first, entry.second);
        }
        renderer_.MakeImage(svg, busses, stops, std::move(svg_options_));
        return svg;
    }
}