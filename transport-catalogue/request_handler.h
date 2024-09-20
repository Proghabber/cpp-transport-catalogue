#pragma once
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"
#include "json_reader.h"
#include <optional>
#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>
#include <set>

namespace handler {
    class RequestHandler {
    public:
        RequestHandler(catalogue::TransportCatalogue& db, render::SvgMaker& map, readJson::JsonReader& json, transport_router::Transport_Router& rout);
        void ReadJson(std::istream& input);
        void ReturnJson(std::ostream& output);
        const std::map<std::string, double> GetRoutSettings() const;
    private:
        catalogue::TransportCatalogue& db_;
        render::SvgMaker& renderer_;
        readJson::JsonReader& reader_;
        transport_router::Transport_Router& router_;

        render::SvgOption svg_options_;
        std::map<std::string, double> rout_settings_;
        
        data_handler::AllRequest in_transport_ ; // запросы для транспорта ввод
        std::vector<data_handler::AllInfo> collect_answer_;// ответы на запросы
        std::vector<data_handler::RetRequest> out_requests_; // все запросы на вывод

        std::ostringstream MakeImage();
        void FullTransport();
        std::pair<data_bus::InfoStop,int> GetStopResponse(const data_handler::RetRequest& request);
        std::pair<data_bus::InfoBus,int> GetBusResponse (const data_handler::RetRequest& request);
        data_handler::MapRequest GetMapResponse(const data_handler::RetRequest& request);
        data_handler::RoutResponse GetRoutResponse(const data_handler::RetRequest& request);
        void GetResponses();
         
        
    };
}


