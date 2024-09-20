#include "json_reader.h"

namespace readJson{
    void JsonReader::ParseDistance(json::Dict& dist, data_handler::StopRequest& stop ){
        for (const std::pair<const std::string, json::Node>& elem : dist){
           stop.distance[elem.first] = elem.second.AsInt();
        }
    }

    data_handler::StopRequest JsonReader::ParseStation(json::Dict &dict){   
        json::Dict distance = dict.at("road_distances").AsDict();
        data_handler::StopRequest stop;
        stop.name = dict.at("name").AsString();
        stop.point = {dict.at("latitude").AsDouble(), dict.at("longitude").AsDouble()};
        ParseDistance(distance, stop); 
        return stop;
    }

    data_handler::BusRequest JsonReader::ParseBus(json::Dict& dict){
        data_handler::BusRequest bus;
        bus.name = dict.at("name").AsString();
        bus.is_roundtrip = dict.at("is_roundtrip").AsBool();
        json::Array stops = dict.at("stops").AsArray();
        for (json::Node& stop: stops){
            bus.stops.push_back(stop.AsString());
        }
        return bus;
    }

    void JsonReader::ParseInput(data_handler::AllRequest& in_requests){       
        for(const auto& dict : base_requests_){
            if (dict.AsDict().at("type").AsString() == "Bus"){
                json::Dict bus_map = dict.AsDict();
                in_requests.buses.push_back(ParseBus(bus_map));             
            }
            if (dict.AsDict().at("type").AsString() == "Stop"){  
                json::Dict stop_map = dict.AsDict();
                in_requests.stops.push_back(ParseStation(stop_map));                 
            }         
        }
    }

    void JsonReader::ParseOutput(std::vector<data_handler::UniversalRequest>& out_requests){
        for (const json::Node& dict: stat_requests_){ 
            json::Dict bus_map = dict.AsDict();
            data_handler::UniversalRequest req;
            req.id = bus_map.at("id").AsInt();
            req.request_type =  bus_map.at("type").AsString();
            if (bus_map.count("name")){
                req.request_name = bus_map.at("name").AsString();
            }
            if (bus_map.count("from")){
                req.from = bus_map.at("from").AsString();
            }           
            if (bus_map.count("to")){
                req.to = bus_map.at("to").AsString();
            }      
            out_requests.push_back(req);       
        }
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

    void JsonReader::ParseRenderSetting(render::SvgOption& svg_options){    
        svg_options.width = render_setting_.at("width").AsDouble();
        svg_options.height = render_setting_.at("height").AsDouble();
        svg_options.padding = render_setting_.at("padding").AsDouble();
        svg_options.stop_radius = render_setting_.at("stop_radius").AsDouble();
        svg_options.line_width = render_setting_.at("line_width").AsDouble();
        svg_options.bus_label_font_size = render_setting_.at("bus_label_font_size").AsInt();
        svg_options.stop_label_font_size = render_setting_.at("stop_label_font_size").AsInt();
        
        svg_options.underlayer_width = render_setting_.at("underlayer_width").AsDouble(); 
        svg_options.underlayer_color = ToColor(render_setting_.at("underlayer_color"));
        for (json::Node nod : render_setting_.at("color_palette").AsArray()){
            svg_options.color_palette.push_back(ToColor(nod));
        }
        for (json::Node nod : render_setting_.at("bus_label_offset").AsArray()){
            svg_options.bus_label_offset.push_back(nod.AsDouble());
        }
        for (json::Node nod : render_setting_.at("stop_label_offset").AsArray()){
            svg_options.stop_label_offset.push_back(nod.AsDouble());
        }
    }

    void JsonReader::ParseRoutsSettings(transport_router::RouteParameters& router_parametrs){
        router_parametrs.bus_wait_time = router_settings_.at("bus_wait_time").AsInt();
        router_parametrs.bus_velocity = router_settings_.at("bus_velocity").AsDouble();  
    }

    void JsonReader::CreateJsonBus(std::vector<json::Node>& nodes,const data_handler::AllInfo& data ){
        using namespace std::literals;
        std::pair<data_bus::InfoBus,int> answer = std::get<std::pair<data_bus::InfoBus,int>>(data);
        if (answer.first.IsEmpty()){
            json::Node dict_node = {json::Builder{}
                                    .StartDict()
                                        .Key("request_id"s).Value(answer.second)
                                        .Key("error_message"s).Value("not found"s)
                                    .EndDict()
                                    .Build()
                                    };
            nodes.push_back(dict_node);
        } else {
            json::Node dict_node = {json::Builder{}
                                    .StartDict()
                                        .Key("curvature"s).Value(answer.first.curvature)
                                        .Key("request_id"s).Value(answer.second)
                                        .Key("route_length"s).Value(answer.first.distance)
                                        .Key("stop_count"s).Value(static_cast<int>(answer.first.amount))
                                        .Key("unique_stop_count"s).Value(static_cast<int>(answer.first.unique))
                                    .EndDict()
                                    .Build()
                                    };
            nodes.push_back(dict_node);
        }
    }

    void JsonReader::CreateJsonStop(std::vector<json::Node>& nodes, const data_handler::AllInfo& data){
        using namespace std::literals;
        std::pair<data_bus::InfoStop,int> response = std::get<std::pair<data_bus::InfoStop,int>>(data);
        if (response.first.name == ""){
            json::Node dict_node = {json::Builder{}
                                    .StartDict()
                                        .Key("request_id"s).Value(response.second)
                                        .Key("error_message"s).Value("not found"s)
                                    .EndDict()
                                    .Build()
                                    };
            nodes.push_back(dict_node);
        } else if (response.first.IsEmpty()){
            json::Node dict_node = {json::Builder{}
                                    .StartDict()
                                        .Key("buses"s).StartArray()
                                                    .EndArray()
                                        .Key("request_id"s).Value(response.second)
                                    .EndDict()
                                    .Build()
                                    };
            nodes.push_back(dict_node);
        } else if (!response.first.IsEmpty()){
                json::Array buses_arr;
                for (const std::string_view stop_name : response.first.stops){
                    buses_arr.push_back(std::string(stop_name));
                }
                json::Node dict_node = {json::Builder{}
                                        .StartDict()
                                            .Key("buses"s).Value(buses_arr)
                                            .Key("request_id"s).Value(response.second)
                                        .EndDict()
                                        .Build()
                                        };
                nodes.push_back(dict_node);
        }
     }

    void JsonReader::CreateJsonMap(std::vector<json::Node>& nodes,const data_handler::AllInfo& data){
        using namespace std::literals;
        const data_handler::MapRequest& response = std::get<data_handler::MapRequest>(data);
        if (response.answer){
            
            json::Node dict_node = {json::Builder{}
                                    .StartDict()
                                        .Key("map"s).Value(response.svg.str())
                                        .Key("request_id"s).Value(response.id)
                                    .EndDict()
                                    .Build()
                                    };
            nodes.push_back(dict_node);
        }
     }

    void JsonReader::CreateJsonRout(std::vector<json::Node> &nodes,const data_handler::AllInfo& data){
        using namespace std::literals;
        const data_handler::RouteSearchResponse& response = std::get<data_handler::RouteSearchResponse>(data);
        if (!response.full){
            json::Node dict_node = {json::Builder{}
                                    .StartDict()
                                        .Key("request_id"s).Value(response.request_info.id)
                                        .Key("error_message").Value(response.massage)
                                    .EndDict()
                                    .Build()
                                    };
            nodes.push_back(dict_node);
        } else {
            std::vector<json::Node> sub_nodes;
            for (size_t index = 0; index < response.bus_stop_count.size(); index++){
                json::Node dict_stop = {json::Builder{}
                                    .StartDict()
                                        .Key("stop_name").Value(std::string(response.stops.at(index)))
                                        .Key("time").Value(response.wait_time)
                                        .Key("type").Value("Wait")
                                    .EndDict()
                                    .Build()
                                    };
                sub_nodes.push_back(dict_stop);
                json::Node dict_bus = {json::Builder{}
                                    .StartDict()
                                        .Key("bus").Value(std::string(response.buses.at(index)))
                                        .Key("span_count").Value(response.bus_stop_count.at(index))
                                        .Key("time").Value(response.time_go.at(index))
                                        .Key("type").Value("Bus")
                                    .EndDict()
                                    .Build()        
                                    };
                sub_nodes.push_back(dict_bus);
            }
            json::Node dict_node = {json::Builder{}
                                    .StartDict()
                                        .Key("request_id").Value(response.request_info.id)
                                        .Key("total_time").Value(response.all_time_go)
                                        .Key("items"s).Value(sub_nodes)
                                    .EndDict()
                                    .Build()
                                    };
            nodes.push_back(dict_node);
        }
     }

    json::Node  JsonReader::ReturnAnswer(const std::vector<data_handler::AllInfo>& responses){
        std::vector<json::Node> nodes;
        for (const data_handler::AllInfo& response: responses){
            if (std::holds_alternative<std::pair<data_bus::InfoBus,int>>(response)){ //автобус
                CreateJsonBus(nodes, response);
            } else if (std::holds_alternative<std::pair<data_bus::InfoStop,int>>(response)){ //остановка
                CreateJsonStop(nodes, response);
            } else if (std::holds_alternative<data_handler::MapRequest>(response)){ //карта
                CreateJsonMap(nodes, response);
            } else if (std::holds_alternative<data_handler::RouteSearchResponse>(response)){ // маршрут, который надо нарисовать по двум остановкам
                CreateJsonRout(nodes, response);
            }            
        } 
        json::Node json{nodes};
        return json;
    }

    void JsonReader::Print(std::ostream& output, const std::vector<data_handler::AllInfo>& responses){
        std::ostringstream out;
        json::Node node = ReturnAnswer(responses);
        json::Print(json::Document{node}, out);
        output<<out.str();
    }

    void JsonReader::ParseRequests(std::istream& input){
        json::Document doc = json::Load(input);//создаем документ из  обьекта
        json::Dict dict = doc.GetRoot().AsDict();//получаем мап 
        base_requests_ = dict.at("base_requests").AsArray();// базовые запросы
        stat_requests_ = dict.at("stat_requests").AsArray();// запросы возврата ответа
        render_setting_ = dict.at("render_settings").AsDict();// запросы настроек 
        router_settings_ = dict.at("routing_settings").AsDict();// запросы маршрута 
    }
}


