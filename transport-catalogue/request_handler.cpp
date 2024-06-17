#include "request_handler.h"
#include <cassert>



/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */


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


void RequestHandler::SaveRequest(handler::AllRequest&& requests){   
    for (handler::stop_request& stop: requests.stops){
        AddStop(stop.name,stop.point);
        for (std::pair<const std::string, int> dist: stop.distance){
            AddStopsDistance(stop.name, dist.first, dist.second);
        }
    }
    for (handler::bus_request& bus: requests.buses){
        AddBus(bus.name, RetRefferense(bus.stops), bus.is_roundtrip);
    }

}

void RequestHandler::SaveAnswers(std::vector<handler::RetRequest>&& requests_vec){
    out_requests = std::move(requests_vec);
}

void RequestHandler::SaveSvgOption(render::SvgOption&& requests_svg){
    svg_options = std::move(requests_svg);
}

std::vector<handler::AllInfo> RequestHandler::GetAnswers(){
    std::vector<AllInfo> collect_answer;
    for (const RetRequest& req: out_requests){
        if (req.type == "Stop"){
            int id = req.id;
            catalogue::InfoStop bus(GetStopStat(req.name));
            std::pair<catalogue::InfoStop,int> nat;
            nat.first = bus;
            nat.second = id;
            AllInfo noy(nat);
            collect_answer.push_back(noy);
        } else if (req.type == "Bus"){
            int id = req.id;
            catalogue::InfoBus bus(GetBusStat(req.name));
            std::pair<catalogue::InfoBus,int> nat;
            nat.first = bus;
            nat.second = id;
            AllInfo noy(nat);
            collect_answer.push_back(noy);
        } else if (req.type == "Map"){
            map_request map;
            map.answer = true;
            map.id = req.id;
            AllInfo noy(map);
            collect_answer.push_back(noy);
        }
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

catalogue::InfoBus RequestHandler::GetBusStat(const std::string_view &bus_name) const{
    return db_.FindBus(bus_name);
}
catalogue::InfoStop RequestHandler::GetStopStat(const std::string_view &stop_name) const{
    return db_.FindStop(stop_name);
}
std::vector<geo_math::Coordinates> RequestHandler::SavePoints(const BusMap& busses, const StopMap& stops){   
    std::vector<geo_math::Coordinates> cordinates_all;
    std::set<std::string_view> stops_etalon;
    
    for (std::pair<std::string_view,catalogue::Bus> bus: busses){      
        for (std::string_view stop: bus.second.stops ){
            if (!stops_etalon.count(stop)){
                stops_etalon.insert(stop);
                cordinates_all.push_back(stops.at(stop)->point);
            }
        }
    }
    return cordinates_all;
}

void RequestHandler::CreateLineBus(const render::SphereProjector& svg_cordinates, svg::Document& doc, const BusMap& busses, const StopMap& stops){
    size_t number_color = 0;
    const size_t max_number_color = svg_options.color_palette.size();
    for (std::pair<std::string_view, catalogue::Bus> bus: busses){
        if (number_color == max_number_color){
            number_color = 0;
        }
        bus_color_[bus.first] = number_color;
        doc.Add(renderer_.MakeLine(std::move(ReferryStopPoins(busses, stops, bus.first)), svg_options, svg_cordinates, bus_color_[bus.first]));
        number_color++;    
    }
}

void RequestHandler::CreateTextBus(const render::SphereProjector &svg_cordinates, svg::Document &doc, const BusMap& busses, const StopMap& stops){
    for(std::pair<std::string_view, catalogue::Bus> bus: busses){
        const size_t end_stop = bus.second.stops.size()-1;
        std::vector<geo_math::Coordinates> points = ReferryStopPoins(busses,stops,bus.first);
        doc.Add(renderer_.MakeBackBus(bus.first, svg_options, points.at(0), svg_cordinates));
        doc.Add(renderer_.MakeBus(bus.first, svg_options, points.at(0), svg_cordinates, bus_color_.at(bus.first)));
        if (bus.second.is_roundtrip == false && *bus.second.stops.begin() != *(bus.second.stops.begin() + end_stop)){
            doc.Add(renderer_.MakeBackBus(bus.first, svg_options, points.at(end_stop), svg_cordinates));
            doc.Add(renderer_.MakeBus(bus.first, svg_options, points.at(end_stop), svg_cordinates, bus_color_.at(bus.first)));
        }
    }
}

void RequestHandler::CreateTextStop(const render::SphereProjector& svg_cordinates, svg::Document& doc, const BusMap& busses, const StopMap& stops){
    std::set<std::string_view> name_stops;
    for (std::pair<std::string_view, catalogue::Bus> bus: busses){
        name_stops.insert(bus.second.stops.begin(),bus.second.stops.end());
    }
    for (std::pair<std::string_view, catalogue::Stop *> stop : stops){
        if (name_stops.count(stop.first)){
            doc.Add(renderer_.MakeBackStop(stop.first, svg_options, stop.second->point, svg_cordinates));
            doc.Add(renderer_.MakeStop(stop.first, svg_options, stop.second->point, svg_cordinates));
        }
    }
}

std::vector<geo_math::Coordinates> RequestHandler::ReferryStopPoins(const BusMap& busses, const StopMap& stops, const std::string_view bus){
    std::vector<geo_math::Coordinates> points;  
        std::vector<std::string_view>  stops_name = busses.at(bus).MakeFullBus();
        for (std::string_view stop : stops_name){
            points.push_back(stops.at(stop)->point);
        }
    return points;
}



void RequestHandler::CreateLableStops(const render::SphereProjector& svg_cordinates, svg::Document& doc, const BusMap& busses, const StopMap& stops){
    std::set<std::string_view> name_stops;
    for (std::pair<std::string_view, catalogue::Bus> bus: busses){
        name_stops.insert(bus.second.stops.begin(),bus.second.stops.end());
    }
    for (std::pair<std::string_view, catalogue::Stop *> stop : stops){
        if (name_stops.count(stop.first)){
            doc.Add(renderer_.MakeLable(svg_options,svg_cordinates, stop.second->point));
        }  
    }  
}

void RequestHandler::MakeImage(std::ostream& out)
{
    BusMap busses;
    StopMap stops;
    
    for (const auto& entry : db_.GetAllBus()) {
        busses.emplace(entry.first, entry.second);
    }
     for (const auto& entry :db_.GetAllstops()) {
        stops.emplace(entry.first, entry.second);
    }

    std::vector<geo_math::Coordinates> cordinates_all = std::move(SavePoints(busses, stops));

    render::SphereProjector svg_cordinates = renderer_.CollectCordins(cordinates_all.begin(), cordinates_all.end()
                                                                        ,svg_options.width
                                                                        ,svg_options.height
                                                                        ,svg_options.padding);
                                                        
    svg::Document doc;
    //линии маршрутов
    CreateLineBus(svg_cordinates, doc, busses, stops);
    
    // надписи маршрутов
    CreateTextBus(svg_cordinates, doc, busses, stops);

    // точки
    CreateLableStops(svg_cordinates, doc, busses, stops);

    //названия остановок
    CreateTextStop(svg_cordinates, doc, busses, stops);
    
    //std::cout<<12;
    doc.Render(out);
    

}

 }