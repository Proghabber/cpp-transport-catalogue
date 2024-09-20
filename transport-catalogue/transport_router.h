#pragma once

#include <vector>
#include <unordered_map>
#include <deque>
#include "transport_catalogue.h"
#include "router.h"
#include "graph.h"
#include "domain.h"


namespace transport_router{ 
    class Transport_Router {
    public:
        Transport_Router(const catalogue::TransportCatalogue& db);
        void SetSettings(std::map<std::string, double>& settings);
        void FullGraph();
        data_handler::RoutResponse FindPath(const data_handler::RetRequest& request);
    private:
        std::unique_ptr<graph::DirectedWeightedGraph<double>>  graph_ = nullptr;
        std::unique_ptr<graph::Router<double>> rout_ = nullptr;
        std::deque<std::string_view> stops_;
        std::unordered_map<std::string_view, size_t> stops_index_;
        std::unordered_map<size_t, AdgeInfo> adge_index_;
        const catalogue::TransportCatalogue& transport_;
        std::map<std::string, double> rout_settings_;
        
        void FullStops(const data_bus::CollectStops& all_stops);
        graph::Edge<double> EdgeStop(std::string_view stop_name);
        graph::Edge<double> EdgeBus(std::pair<std::string_view, std::string_view> path, const double distance);
        graph::Edge<double> CreateAdge(std::pair<std::string_view, std::string_view> path, double distance);    
        void GoToEnd(graph::DirectedWeightedGraph<double>& graph, StopIter parent, std::string_view bus, StopIter end);  
        void GoToBegin(graph::DirectedWeightedGraph<double>& graph, StopIter parent, std::string_view bus, StopIter begin);
        graph::DirectedWeightedGraph<double> CreateGraph();
        data_handler::RoutResponse MakePath(EdgeIter begin, EdgeIter end);
    };
}