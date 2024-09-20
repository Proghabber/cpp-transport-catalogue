#pragma once
#include <optional>
#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>
#include <set>

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"
#include "json_reader.h"

namespace handler {
    class RequestHandler {
    public:
        RequestHandler(catalogue::TransportCatalogue& db, render::SvgMaker& map, readJson::JsonReader& json);
        void ReadJson(std::istream& input);
        void ReturnJson(std::ostream& output);
    private:
        render::SvgOption svg_options_;
        transport_router::RouteParameters router_settings_;
        data_handler::AllRequest in_transport_ ; // запросы для транспорта ввод
        std::vector<data_handler::AllInfo> collect_answer_;// ответы на запросы
        std::vector<data_handler::UniversalRequest> out_requests_; // все запросы на вывод
        catalogue::TransportCatalogue& db_;
        render::SvgMaker& renderer_;
        readJson::JsonReader& reader_;
        transport_router::TransportRouter router_;

        std::ostringstream MakeImage();
        void FullTransport();
        std::pair<data_bus::InfoStop,int> GetStopResponse(const data_handler::UniversalRequest& request);
        std::pair<data_bus::InfoBus,int> GetBusResponse (const data_handler::UniversalRequest& request);
        data_handler::MapRequest GetMapResponse(const data_handler::UniversalRequest& request);
        data_handler::RouteSearchResponse GetRoutResponse(const data_handler::UniversalRequest& request);
        void GetResponses();
         
        
    };
}


