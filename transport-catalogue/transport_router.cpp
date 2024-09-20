#include "transport_router.h"
namespace transport_router{ 
    Transport_Router::Transport_Router(const catalogue::TransportCatalogue& db): transport_(db) 
    {
    }

    void Transport_Router::SetSettings(std::map<std::string, double>& settings){
        rout_settings_ = settings;
    }

    void Transport_Router::FullGraph(){
            graph_ = std::make_unique<graph::DirectedWeightedGraph<double>>(CreateGraph());
            graph::Router<double> rout(*graph_.get());
            rout_ = std::make_unique<graph::Router<double>>(rout);
    }

    data_handler::RoutResponse Transport_Router::FindPath(const data_handler::RetRequest& request){ 
        auto path = rout_->BuildRoute(stops_index_.at(request.from), stops_index_.at(request.to));
        if(!path.has_value()){
            data_handler::RoutResponse response;
            response.full = false;
            response.massage = "not found";
            return response;
        } else {
            data_handler::RoutResponse response = MakePath(path.value().edges.begin(), path.value().edges.end());
            return response;
        }
    }

    void Transport_Router::FullStops(const data_bus::CollectStops& all_stops){
        for (auto stop: all_stops){
            stops_.push_back(stop.first);
            size_t index = stops_.size()-1;
            stops_index_[stops_.at(index)] = index;
        }    
    }

    graph::Edge<double> Transport_Router::EdgeStop(std::string_view stop_name){
        size_t from_index = stops_index_.at(stop_name);
        graph::Edge<double> edge;
        edge.weight = rout_settings_.at("bus_wait_time");
        edge.from = from_index;
        edge.to = from_index;
        return edge;
    }

    graph::Edge<double> Transport_Router::EdgeBus(std::pair<std::string_view, std::string_view> path, const double distance){
        const double metres = 1000.0;
        const double minuts = 60.0;
        double speed = rout_settings_.at("bus_velocity");
        double wait = rout_settings_.at("bus_wait_time");
        size_t from_index = stops_index_.at(path.first);
        size_t to_index = stops_index_.at(path.second);
        graph::Edge<double> edge;
        edge.weight = (distance / metres / speed * minuts) + wait;
        edge.from = from_index;
        edge.to = to_index;
        return edge;
    }

    graph::Edge<double> Transport_Router::Transport_Router::CreateAdge(std::pair<std::string_view, std::string_view> path, double distance){
        graph::Edge<double> edge;
        if (distance <= 0){
            edge = EdgeStop(path.first);
        } else {
            edge = EdgeBus(path, distance);
        }
        return edge;
    }

    void Transport_Router::GoToEnd(graph::DirectedWeightedGraph<double>& graph, StopIter parent, std::string_view bus, StopIter end){
        int stops_count = 0; 
        double all_dist = 0;
        for (auto next_stop = next(parent, 1); next_stop != end; next_stop++){
            stops_count++;
            all_dist += transport_.GetStopsDistanse(*prev(next_stop, 1), *next_stop); 
            size_t index = graph.AddEdge(CreateAdge({*parent, *next_stop}, all_dist)); 
            adge_index_[index] = {stops_count, bus, *parent};
        }
    }

    void Transport_Router::GoToBegin(graph::DirectedWeightedGraph<double>& graph, StopIter parent, std::string_view bus, StopIter begin){
        int stops_count = 0; 
        double all_dist = 0;
        for (auto prev_stop = prev(parent, 1); prev_stop >= begin; prev_stop--){
            stops_count++;
            all_dist += transport_.GetStopsDistanse(*next(prev_stop, 1), *prev_stop);
            size_t index = graph.AddEdge(CreateAdge({*parent, *prev_stop}, all_dist));
            adge_index_[index] = {stops_count, bus, *parent};          
        }
    }

    graph::DirectedWeightedGraph<double> Transport_Router::CreateGraph(){ 
        const data_bus::CollectBus& all_bus = transport_.GetAllBus();
        const data_bus::CollectStops& all_stops = transport_.GetAllstops();
        graph::DirectedWeightedGraph<double>  graph(all_stops.size());
        FullStops(all_stops);
        for (auto stop: all_stops){
            size_t index = graph.AddEdge(CreateAdge({stop.first, stop.first}, 0));
            adge_index_[index] = {0, "", stop.first};
        }
        for (auto bus: all_bus){ 
            std::vector<std::string_view> stops_list = bus.second.stops;
            for (auto stop = stops_list.begin(); stop != stops_list.end(); stop++){
                if (bus.second.is_roundtrip){
                    GoToEnd(graph, stop, bus.first, stops_list.end());
                } else {
                    GoToBegin(graph, stop, bus.first, stops_list.begin());
                    GoToEnd(graph, stop, bus.first, stops_list.end());     
                }       
            }
        }   
        return graph;
    }

    data_handler::RoutResponse Transport_Router::MakePath(EdgeIter begin, EdgeIter end){
        data_handler::RoutResponse response;
        response.full = true;
        if (begin == end){ 
            return response;
        }
        for(auto index = begin; index != end; index++){
            const graph::Edge<double>& adge = graph_->GetEdge(*index);
            response.stops.push_back(adge_index_.at(*index).first_stop);
            response.buses.push_back(adge_index_.at(*index).bus_name);
            response.bus_stop_count.push_back(adge_index_.at(*index).stop_count);
            response.time_go.push_back(adge.weight - rout_settings_.at("bus_wait_time"));
            response.wait_time = rout_settings_.at("bus_wait_time");
            response.all_time_go += adge.weight;
            response.speed = rout_settings_.at("bus_velocity");  
        }
        return response;
    } 
}
