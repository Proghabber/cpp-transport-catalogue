#include "json_reader.h"

namespace readJson{
    void JsonReader::ParseDistance(json::Dict& dist, data_handler::Stop_request& stop ){
        for (const std::pair<const std::string, json::Node>& elem : dist){
           stop.distance[elem.first] = elem.second.AsInt();
        }
    }

    data_handler::Stop_request JsonReader::ParseStation(json::Dict &dict){   
        json::Dict distance = dict.at("road_distances").AsMap();
        data_handler::Stop_request stop;
        stop.name = dict.at("name").AsString();
        stop.point = {dict.at("latitude").AsDouble(), dict.at("longitude").AsDouble()};
        ParseDistance(distance, stop); 
        return stop;
    }

     data_handler::Bus_request JsonReader::ParseBus(json::Dict& dict){
        data_handler::Bus_request bus;
        bus.name = dict.at("name").AsString();
        bus.is_roundtrip = dict.at("is_roundtrip").AsBool();
        json::Array stops = dict.at("stops").AsArray();
        for (json::Node& stop: stops){
            bus.stops.push_back(stop.AsString());
        }
        return bus;
    }

    void JsonReader::ParseInput(json::Array &array, handler::RequestHandler& saver){       
        data_handler::AllRequest in_requests;
        for(const auto& dict : array){
            if (dict.AsMap().at("type").AsString() == "Bus"){
                json::Dict bus_map = dict.AsMap();
                in_requests.buses.push_back(ParseBus(bus_map));             
            }
            if (dict.AsMap().at("type").AsString() == "Stop"){  
                json::Dict stop_map = dict.AsMap();
                in_requests.stops.push_back(ParseStation(stop_map));                 
            }         
        }
        saver.SaveRequest(std::move(in_requests));
    }

   void JsonReader::ParseOutput(json::Array& array, handler::RequestHandler& saver){
       std::vector<data_handler::RetRequest> out_requests;
        for (const json::Node& dict: array){ 
            json::Dict bus_map = dict.AsMap();
            data_handler::RetRequest req;
            req.id = bus_map.at("id").AsInt();
            req.type =  bus_map.at("type").AsString();
            if (bus_map.count("name")){
                req.name = bus_map.at("name").AsString();
            }              
            out_requests.push_back(req);       
        }
        saver.SaveAnswers(std::move(out_requests));
    }

    svg::Color JsonReader::ToColor(json::Node& node){
        svg::Color color;
        if (node.IsString()){
            color = node.AsString();
        }
        if (node.IsArray()){
            json::Array nodes = node.AsArray();
            if (nodes.size() == 3){
                svg::Rgb rgb;
                rgb.blue = static_cast<uint8_t>(nodes.at(2).AsInt());
                rgb.green =static_cast<uint8_t>(nodes.at(1).AsInt());
                rgb.red = static_cast<uint8_t>(nodes.at(0).AsInt());
                color = rgb;
            } else if (nodes.size() == 4){
                svg::Rgba rgba;
                rgba.opacity = nodes.at(3).AsDouble();
                rgba.blue = static_cast<uint8_t>(nodes.at(2).AsInt());
                rgba.green =static_cast<uint8_t>(nodes.at(1).AsInt());
                rgba.red = static_cast<uint8_t>(nodes.at(0).AsInt());
                color = rgba;
            }
        }
        return color;
    }

    void JsonReader::ParseRenderSetting(json::Dict& dict, handler::RequestHandler& saver){   
        render::SvgOption svg_options; 
        svg_options.width = dict.at("width").AsDouble();
        svg_options.height = dict.at("height").AsDouble();
        svg_options.padding = dict.at("padding").AsDouble();
        svg_options.stop_radius = dict.at("stop_radius").AsDouble();
        svg_options.line_width = dict.at("line_width").AsDouble();
        svg_options.bus_label_font_size = dict.at("bus_label_font_size").AsInt();
        svg_options.stop_label_font_size = dict.at("stop_label_font_size").AsInt();
        
        svg_options.underlayer_width = dict.at("underlayer_width").AsDouble(); 
        svg_options.underlayer_color = ToColor(dict.at("underlayer_color"));
        for (json::Node nod : dict.at("color_palette").AsArray()){
            svg_options.color_palette.push_back(ToColor(nod));
        }
        for (json::Node nod : dict.at("bus_label_offset").AsArray()){
            svg_options.bus_label_offset.push_back(nod.AsDouble());
        }
        for (json::Node nod : dict.at("stop_label_offset").AsArray()){
            svg_options.stop_label_offset.push_back(nod.AsDouble());
        }
        saver.SaveSvgOption(std::move(svg_options));
    }

    std::string JsonReader::Print(handler::RequestHandler& saver){
        std::ostringstream out;
        json::Node node = ReturnAnswer(saver);
        json::Print(json::Document{node}, out);
        return out.str();
    }

    void JsonReader::CreateJsonBus(std::vector<json::Node>& nodes, data_handler::AllInfo& data ){
        using namespace std::literals;
        std::pair<data_bus::InfoBus,int> answer = std::get<std::pair<data_bus::InfoBus,int>>(data);
        if (answer.first.IsEmpty()){
            json::Node dict_node{json::Dict{{"request_id"s, answer.second}, {"error_message"s, "not found"s}}};
            nodes.push_back(dict_node);
        } else {
            json::Node dict_node{json::Dict{{"curvature"s, answer.first.curvature},
                                            {"request_id"s, answer.second},
                                            {"route_length"s, answer.first.distance},
                                            {"stop_count"s, static_cast<int>(answer.first.amount)},
                                            {"unique_stop_count"s, static_cast<int>(answer.first.unique)}
                                            }};
            nodes.push_back(dict_node);
        }
    }

     void JsonReader::CreateJsonStop(std::vector<json::Node>& nodes, data_handler::AllInfo& data ){
        using namespace std::literals;
        std::pair<data_bus::InfoStop,int> answer = std::get<std::pair<data_bus::InfoStop,int>>(data);
        if (answer.first.name == ""){
            json::Node dict_node{json::Dict{{"request_id"s, answer.second}, {"error_message"s, "not found"s}}};
            nodes.push_back(dict_node);
        } else if (answer.first.IsEmpty()){
            json::Node dict_node{json::Dict{{"buses"s, json::Array{}}, {"request_id"s, answer.second}}};
            nodes.push_back(dict_node);
        } else if (!answer.first.IsEmpty()){
                json::Array buses_arr;
                for (const std::string_view stop_name : answer.first.stops){
                    buses_arr.push_back(std::string(stop_name));
                }
                json::Node dict_node{json::Dict{{"buses"s, buses_arr}, {"request_id"s, answer.second}}};
                nodes.push_back(dict_node);
        }
     }

     void JsonReader::CreateJsonMap(std::vector<json::Node>& nodes, data_handler::AllInfo& data, handler::RequestHandler& saver ){
        using namespace std::literals;
        data_handler::Map_request answer = std::get<data_handler::Map_request>(data);
        if (answer.answer){
            std::ostringstream svg;
            saver.MakeImage(svg);
            json::Node nod = json::Node(svg.str());
            json::Node dict_node{json::Dict{{"map"s, nod }, {"request_id"s, answer.id}}};
            nodes.push_back(dict_node);
        }
     }

     json::Node  JsonReader::ReturnAnswer(handler::RequestHandler& saver){
        std::vector<json::Node> nodes;
        std::vector<data_handler::AllInfo> answers = saver.GetAnswers();
        for (data_handler::AllInfo& data: answers){
            if (std::holds_alternative<std::pair<data_bus::InfoBus,int>>(data)){ //автобус
                CreateJsonBus(nodes, data);
            } else if (std::holds_alternative<std::pair<data_bus::InfoStop,int>>(data)){ //остановка
                CreateJsonStop(nodes, data);
            } else if (std::holds_alternative<data_handler::Map_request>(data)){ //карта
                CreateJsonMap(nodes, data, saver);
            }       
        } 
        json::Node json{nodes};
        return json;
    }

      void JsonReader::ParseRequests(std::istream& input, handler::RequestHandler& saver){
        json::Document doc = json::Load(input);//создаем документ из  обьекта
        json::Dict dict = doc.GetRoot().AsMap();//получаем мап 
        json::Array base_requests = dict.at("base_requests").AsArray();// базовые запросы
        json::Array stat_requests = dict.at("stat_requests").AsArray();// запросы возврата ответа
        json::Dict render_setting = dict.at("render_settings").AsMap();// запросы настроек 
        //____________________________________________________________________
        ParseInput(base_requests, saver);// парсим запрос базовый 
        ParseOutput(stat_requests, saver);// парсим запрос на возврат ответа
        ParseRenderSetting(render_setting, saver);// парсим настройки изображения
    }
}


