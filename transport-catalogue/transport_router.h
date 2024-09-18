#pragma once

#include <vector>
#include <unordered_map>
#include <deque>
#include "request_handler.h"
#include "router.h"
#include "graph.h"
#include "domain.h"


namespace transport_router{
    std::deque<std::string_view> stops_;
    std::unordered_map<std::string_view, size_t> stops_index_;

    void AddStop(std::string_view stop){
            stops_.push_back(stop);
            size_t index = stops_.size()-1;
            stops_index_[stops_.at(index)] = index;
        }

    template <typename Weight>
    graph::Edge<Weight> EdgeStop(std::string_view stop_name, const std::map<std::string, Weight>& rout_settings_ ){
            size_t from_index = stops_index_.at(stop_name);
            graph::Edge<Weight> edge;
            edge.SetDistance(-1);
            edge.SetInfoStop(stop_name, rout_settings_.at("bus_wait_time"));
            edge.SetPoints(from_index, from_index);
            edge.CountWeight();
            return edge;
        }

    template <typename Weight>
    graph::Edge<Weight> EdgeBus(std::pair<std::string_view, std::string_view> path, const double distance, const int stops_count, std::string_view bus, const std::map<std::string, Weight>& rout_settings_){
        size_t from_index = stops_index_.at(path.first);
        size_t to_index = stops_index_.at(path.second);
        graph::Edge<Weight> edge;
        edge.SetDistance(distance);
        edge.SetInfoStop(path.first, rout_settings_.at("bus_wait_time"));
        edge.SetInfoBus(bus, stops_count, rout_settings_.at("bus_velocity"));
        edge.SetPoints(from_index, to_index);
        edge.CountWeight();
        return edge;
    }

    template <typename Weight>
    graph::Edge<Weight> CreateAdge(std::pair<std::string_view, std::string_view> path, double distance, int stop_count, std::string_view bus, const std::map<std::string, Weight>& rout_settings_){
        graph::Edge<Weight> edge;
        if (distance <= 0){
            edge = EdgeStop(path.first, rout_settings_);
        } else {
            edge = EdgeBus(path, distance, stop_count, bus,rout_settings_);
        }
        return edge;
    }

    template <typename Weight, typename Iterator>
    void GoToEnd(graph::DirectedWeightedGraph<Weight>& graph, Iterator parent, std::string_view bus, Iterator end, const handler::RequestHandler& handl){
        int stops_count = 0; 
        double all_dist = 0;
        for (auto next_stop = next(parent, 1); next_stop != end; next_stop++){
             stops_count++;
             all_dist += handl.GetStopsDist(*prev(next_stop, 1), *next_stop); 
             graph.AddEdge(CreateAdge({*parent, *next_stop}, all_dist, stops_count, bus, handl.GetRoutSettings())); 
         }
    }

    template <typename Weight, typename Iterator>
    void GoToBegin(graph::DirectedWeightedGraph<Weight>& graph, Iterator parent, std::string_view bus, Iterator begin, const handler::RequestHandler& handl){
        int stops_count = 0; 
        double all_dist = 0;
        for (auto prev_stop = prev(parent, 1); prev_stop >= begin; prev_stop--){
            stops_count++;
            all_dist += handl.GetStopsDist(*next(prev_stop, 1), *prev_stop);
            graph.AddEdge(CreateAdge({*parent, *prev_stop}, all_dist, stops_count, bus, handl.GetRoutSettings()));
        }
    }

    template <typename Weight>
    graph::DirectedWeightedGraph<Weight> CreateGraph(const handler::RequestHandler& handl){
        const data_bus::CollectStops& all_stops = handl.GetAllStops();
        graph::DirectedWeightedGraph<Weight>  graph(all_stops.size());

        for (auto stop: all_stops){
            AddStop(stop.first);
        }
        for (auto stop: all_stops){
            graph.AddEdge(CreateAdge({stop.first, stop.first}, -1, 0, "", handl.GetRoutSettings()));
        }
        const data_bus::CollectBus& all_bus = handl.GetAllBus();
        for (auto bus: all_bus){ 
            std::vector<std::string_view> stops_list = bus.second.stops;
            for (auto stop = stops_list.begin(); stop != stops_list.end(); stop++){
                if (bus.second.is_roundtrip){
                    GoToEnd(graph, stop, bus.first, stops_list.end(), handl);
                } else {
                    GoToBegin(graph, stop, bus.first, stops_list.begin(), handl);
                    GoToEnd(graph, stop, bus.first, stops_list.end(), handl);     
                }       
            }
        }   
        return graph;
    }

    template <typename Weight>
    class Transport_Router {
    public:
        Transport_Router(handler::RequestHandler& handl):handl_requests_(handl), graph_(CreateGraph<Weight>(handl)), rout_(graph_)
        {   
        };
        
        void FindPath(){
            for (auto req: handl_requests_.RetRequests()){
                if (req.type != "Route"){
                    continue;
                }
                auto path = rout_.BuildRoute(stops_index_.at(req.from), stops_index_.at(req.to));
                data_handler::RoutAnswer answer_path;
                if(!path.has_value()){
                    answer_path.full = false;
                    answer_path.massage = "not found";
                    answer_path.id = req.id;
                    handl_requests_.SavePath(answer_path, req.id);
                    continue;
                }
                answer_path = MakePath(path.value().edges.begin(), path.value().edges.end());
                answer_path.id = req.id;
                handl_requests_.SavePath(answer_path, req.id);
            }  
        }

    private:
        handler::RequestHandler& handl_requests_;
        const graph::DirectedWeightedGraph<Weight>  graph_;
        graph::Router<Weight> rout_;

        template <typename iterator>
        data_handler::RoutAnswer MakePath(iterator begin, iterator end){
            data_handler::RoutAnswer answer;
            answer.full = true;
            answer.all_time_go = 0.0;
            if (begin == end){ 
                return answer;
            }
            for(auto index = begin; index != end; index++){
                const graph::Edge<Weight>& adge = graph_.GetEdge(*index);
                answer.stops.push_back(adge.first_stop);
                answer.buses.push_back(adge.bus);
                answer.bus_stop_count.push_back(adge.count_stop);
                answer.time_go.push_back(adge.weight- adge.bus_wait_time);
                answer.wait_time = adge.bus_wait_time;
                answer.all_time_go += adge.weight ;
                answer.speed = adge.bus_velocity;  
            }
            return answer;
        } 
    };
}