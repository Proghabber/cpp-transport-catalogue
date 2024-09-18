#pragma once

#include "ranges.h"

#include <cstdlib>
#include <vector>

namespace graph {

using VertexId = size_t;
using EdgeId = size_t;


struct EdgeInfo {
    std::string_view bus;
    std::string_view first_stop;
    size_t count_stop;
    size_t time_wait;
};

template <typename Weight>
struct Edge {
    VertexId from;
    VertexId to;
    Weight weight;
    std::string_view bus;
    std::string_view first_stop;
    int count_stop = 0;
    double bus_wait_time = 0.0;
    double bus_velocity = 0.0;

    void SetPoints(const VertexId start, const VertexId finish){
        from = start;
        to = finish;
    }

    void SetDistance(const Weight distance){
        weight = distance;
    }

    void SetInfoBus(std::string_view name, const int stops_count, const double speed){
        bus = name;
        count_stop = stops_count;
        bus_velocity = speed;
    }

    void SetInfoStop(std::string_view name_stop, const double wait_time){
        first_stop = name_stop;
        bus_wait_time = wait_time;
    }

    void CountWeight(){
        if (weight > 0){
            const double metres = 1000.0;
            const double minuts = 60.0;
            weight = (weight / metres / bus_velocity * minuts) + bus_wait_time;

        } else { 
            weight = bus_wait_time;
        };
    }
};


template <typename Weight>
class DirectedWeightedGraph {
private:
    using IncidenceList = std::vector<EdgeId>;
    using IncidentEdgesRange = ranges::Range<typename IncidenceList::const_iterator>;

public:
    DirectedWeightedGraph() = default;
    explicit DirectedWeightedGraph(size_t vertex_count);
    EdgeId AddEdge(const Edge<Weight>& edge);

    size_t GetVertexCount() const;
    size_t GetEdgeCount() const;
    const Edge<Weight>& GetEdge(EdgeId edge_id) const;
    IncidentEdgesRange GetIncidentEdges(VertexId vertex) const;

private:
    std::vector<Edge<Weight>> edges_;
    std::vector<IncidenceList> incidence_lists_;
};

template <typename Weight>
DirectedWeightedGraph<Weight>::DirectedWeightedGraph(size_t vertex_count)
    : incidence_lists_(vertex_count) {
}

template <typename Weight>
EdgeId DirectedWeightedGraph<Weight>::AddEdge(const Edge<Weight>& edge) {
    edges_.push_back(edge);
    const EdgeId id = edges_.size() - 1;
    incidence_lists_.at(edge.from).push_back(id);
    return id;
}

template <typename Weight>
size_t DirectedWeightedGraph<Weight>::GetVertexCount() const {
    return incidence_lists_.size();
}

template <typename Weight>
size_t DirectedWeightedGraph<Weight>::GetEdgeCount() const {
    return edges_.size();
}

template <typename Weight>
const Edge<Weight>& DirectedWeightedGraph<Weight>::GetEdge(EdgeId edge_id) const {
    return edges_.at(edge_id);
}

template <typename Weight>
typename DirectedWeightedGraph<Weight>::IncidentEdgesRange
DirectedWeightedGraph<Weight>::GetIncidentEdges(VertexId vertex) const {
    return ranges::AsRange(incidence_lists_.at(vertex));
}
}  // namespace graph