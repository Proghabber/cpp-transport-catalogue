#include "request_handler.h"

namespace handler {

RequestHandler::RequestHandler(catalogue::TransportCatalogue& db, render::SvgMaker& map):db_(db), renderer_(map)
{
}

 std::vector<std::string_view> RetRefferense(std::vector<std::string>& text){
    std::vector<std::string_view> refferens;
    for (std::string& ref: text){
        refferens.push_back(ref);
    }
    return refferens;
}


void RequestHandler::SaveRequest(data_handler::AllRequest&& requests){   
    for (data_handler::stop_request& stop: requests.stops){
        AddStop(stop.name,stop.point);
        for (std::pair<const std::string, int> dist: stop.distance){
            AddStopsDistance(stop.name, dist.first, dist.second);
        }
    }
    for (data_handler::bus_request& bus: requests.buses){
        std::vector<std::string_view> rezult;
        rezult.reserve(bus.stops.size());
        auto fun =[](const std::string& stop){return stop;};
        std::transform(bus.stops.cbegin(), bus.stops.cend(), std::back_inserter(rezult), fun);
        AddBus(bus.name, RetRefferense(bus.stops), bus.is_roundtrip);
    }
}

void RequestHandler::SaveAnswers(std::vector<data_handler::RetRequest>&& requests_vec){
    out_requests = std::move(requests_vec);
}

void RequestHandler::SaveSvgOption(render::SvgOption&& requests_svg){
    svg_options_ = std::move(requests_svg);
}

std::vector<data_handler::AllInfo> RequestHandler::GetAnswers(){
    std::vector<data_handler::AllInfo> collect_answer;
    for (const data_handler::RetRequest& req: out_requests){
        data_handler::AllInfo answer;
        if (req.type == "Stop"){
            int id = req.id;
            data_bus::InfoStop stop(GetStopStat(req.name));
            std::pair<data_bus::InfoStop,int> object;
            object.first = stop;
            object.second = id;
            answer = object;
        } else if (req.type == "Bus"){
            int id = req.id;
            data_bus::InfoBus bus(GetBusStat(req.name));
            std::pair<data_bus::InfoBus,int> object;
            object.first = bus;
            object.second = id;
            answer = object;
        } else if (req.type == "Map"){
            data_handler::map_request object;
            object.answer = true;
            object.id = req.id;
            answer = object;
        }
        collect_answer.push_back(answer);
    }
    return collect_answer;
}

void RequestHandler::AddStop(const std::string& id, geo_math::Coordinates point){
    db_.AddStop(id, point);
}

void RequestHandler::AddStopsDistance(const std::string_view stop_one, const std::string_view stop_two, const int distance){
    db_.AddStopsDistance(stop_one, stop_two, distance);
}

void RequestHandler::AddBus(const std::string& id, const std::vector<std::string_view> stops, bool is_roundtrip){
    db_.AddBus(id, stops, is_roundtrip);
}

data_bus::InfoBus RequestHandler::GetBusStat(const std::string_view &bus_name) const{
    return db_.FindBus(bus_name);
}

data_bus::InfoStop RequestHandler::GetStopStat(const std::string_view &stop_name) const{
    return db_.FindStop(stop_name);
}

void RequestHandler::MakeImage(std::ostream& out)
{
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
 }