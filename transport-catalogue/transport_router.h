#pragma once

#include <vector>
#include <unordered_map>
#include <deque>

#include "transport_catalogue.h"
#include "router.h"
#include "graph.h"
#include "domain.h"


namespace transport_router{ 
    class TransportRouter {
    public:
        TransportRouter(const catalogue::TransportCatalogue& db, const transport_router::RouteParameters& settings);
        data_handler::RouteSearchResponse FindPath(const data_handler::UniversalRequest& request);
    private:
        std::unique_ptr<graph::DirectedWeightedGraph<double>>  graph_ = nullptr;
        std::unique_ptr<graph::Router<double>> rout_ = nullptr;
        std::deque<std::string_view> stops_;
        std::unordered_map<std::string_view, size_t> stops_index_;
        std::unordered_map<size_t, EdgeInfo> adge_index_;
        const catalogue::TransportCatalogue& transport_;
        const transport_router::RouteParameters& rout_settings_;
        
        void FillGraph();
        void FillStops(const data_bus::CollectStops& all_stops);
        graph::Edge<double> EdgeStop(std::string_view stop_name);
        graph::Edge<double> EdgeBus(std::pair<std::string_view, std::string_view> path, const double distance);
        graph::Edge<double> CreateAdge(std::pair<std::string_view, std::string_view> path, double distance);    
        void GoToEnd(graph::DirectedWeightedGraph<double>& graph, StopIter parent, std::string_view bus, StopIter end);  
        void GoToBegin(graph::DirectedWeightedGraph<double>& graph, StopIter parent, std::string_view bus, StopIter begin);
        graph::DirectedWeightedGraph<double> CreateGraph();
        data_handler::RouteSearchResponse MakePath(EdgeIter begin, EdgeIter end);
    };
}