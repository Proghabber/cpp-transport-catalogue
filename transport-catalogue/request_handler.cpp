#include "request_handler.h"

namespace handler {
    RequestHandler::RequestHandler(catalogue::TransportCatalogue& db, render::SvgMaker& map):db_(db), renderer_(map)
    {
    }

    void RequestHandler::FullTransport(data_handler::AllRequest&& requests){   
        for (data_handler::StopRequest& stop: requests.stops){
            db_.AddStop(stop.name,stop.point);
            
            for (std::pair<const std::string, int> dist: stop.distance){
                db_.AddStopsDistance(stop.name, dist.first, dist.second);
            }
        }
        for (data_handler::BusRequest& bus: requests.buses){
            std::vector<std::string_view> result;
            result.reserve(bus.stops.size());
            auto fun = [](const std::string& stop) -> std::string_view {return stop;};
            std::transform(bus.stops.cbegin(), bus.stops.cend(), std::back_inserter(result), fun);
            db_.AddBus(bus.name, result, bus.is_roundtrip);
        }
    }

    void RequestHandler::SaveAnswers(std::vector<data_handler::RetRequest>&& requests_vec){
        out_requests_ = std::move(requests_vec);
    }

    void RequestHandler::SaveSvgOption(render::SvgOption&& requests_svg){
        svg_options_ = std::move(requests_svg);
    }

    std::vector<data_handler::AllInfo> RequestHandler::GetAnswers(){
        //std::vector<data_handler::AllInfo> collect_answer; 
        for (const data_handler::RetRequest& req: out_requests_){ 
            data_handler::AllInfo answer; 
            if (req.type == "Stop"){ 
                int id = req.id; 
                data_bus::InfoStop stop(db_.FindStop(req.name)); 
                std::pair<data_bus::InfoStop,int> object; 
                object.first = stop; 
                object.second = id; 
                answer = object; 
            } else if (req.type == "Bus"){ 
                int id = req.id; 
                data_bus::InfoBus bus(db_.FindBus(req.name)); 
                std::pair<data_bus::InfoBus,int> object; 
                object.first = bus; 
                object.second = id; 
                answer = object; 
            } else if (req.type == "Map"){ 
                data_handler::MapRequest object; 
                object.answer = true; 
                object.id = req.id; 
                answer = object; 
            } else if (req.type == "Route"){
                answer =  rout_answers_.at(req.id);
            }
            collect_answer_.push_back(answer); 
        } 
        return collect_answer_;
    }

    void RequestHandler::MakeImage(std::ostream& out){
        data_bus::BusMap busses;
        data_bus::StopMap stops;
        
        for (const auto& entry : db_.GetAllBus()) {
            busses.emplace(entry.first, entry.second);
        }
        for (const auto& entry :db_.GetAllstops()) {
            stops.emplace(entry.first, entry.second);
        }
        renderer_.MakeImage(out, busses, stops, std::move(svg_options_));
    }

    void RequestHandler::SaveRoutSettings(std::map<std::string, double>&& set_rout){
        rout_settings_ = std::move(set_rout);
    }

    const std::map<std::string, double> RequestHandler::GetRoutSettings() const {
        return rout_settings_;
    }

    const data_bus::CollectStops& RequestHandler::GetAllStops() const { 
        return db_.GetAllstops();  
    }

    const data_bus::CollectBus& RequestHandler::GetAllBus() const {
        return db_.GetAllBus();
    }

    double RequestHandler::GetStopsDist(std::string_view from, std::string_view to) const {
        return db_.GetStopsDistanse(from, to);
    }

    void RequestHandler::SavePath(data_handler::RoutAnswer path, int id){
        rout_answers_[id] = path;
    }

    std::vector<data_handler::RetRequest>& RequestHandler::RetRequests(){
        return out_requests_;
    }
}