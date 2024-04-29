#include "transport_catalogue.h"



namespace catalogue{
void TransportCatalogue::AddStop(const std::string& id ,geo_math::Coordinates point){
    auto s = std::find(name_stops_.begin(),name_stops_.end(),id);
    if(s !=name_stops_.end() ){
        StopSt t(*s, point);
        stops_.push_back(t); 
        stops_ptr_[*s] = &stops_.back();
    }else{
        name_stops_.push_back(id);
        StopSt t(name_stops_.back(), point);
        stops_.push_back(t); 
        stops_ptr_[name_stops_.back()]=&stops_.back();
       
    }
    
         
}


void TransportCatalogue::AddBus(const std::string& id, const std::vector<std::string_view>& stops){
    std::vector<std::string_view> filtered_stops_vector;   
    for (auto& stop:stops){
        if (stops_ptr_.count(stop)){
            size_t index =std::find(name_stops_.begin(), name_stops_.end(), stop)-name_stops_.begin();
            filtered_stops_vector.push_back(name_stops_[index]);     
        }    
    }
    name_bus_.push_back(id);
    buses_.push_back({name_bus_.back(), filtered_stops_vector});
    bus_ptr_[buses_.back().first] = buses_.back().second;
    for (auto stop : bus_ptr_.at(buses_.back().first)){
        stops_to_buses_[stop].insert(buses_.back().first);
     }
}


 InfoBus TransportCatalogue::CountInfoBetweenStations(std::string_view bus ) const{

    int count = 0;
    size_t count_stops = bus_ptr_.at(bus).size();
    size_t repeat_stop = 0 ;
    std::map<std::string_view, size_t> statistics_repeat;
    
    Coordinates from;
    Coordinates to;
    double distance_geo = 0;
    double distance_track = CountDist(bus);

    for (auto stop : bus_ptr_.at(bus) ){
        ++statistics_repeat[stop];
        to=stops_ptr_.at(stop)->point;    
        if (!count){
            from=stops_ptr_.at(stop)->point;
        }
        if (statistics_repeat[stop] >1 ){
             repeat_stop++;
        }
        distance_geo+=ComputeDistance(to, from);
        from = to;
        count++;
    }
    return {count_stops, count_stops-repeat_stop, distance_geo, distance_track, distance_track/distance_geo};
}

 InfoBus TransportCatalogue::FindBus(std::string_view id) const {
    if (!bus_ptr_.count(id)){ 
        return {};
    }  
    return CountInfoBetweenStations(id);
}

std::optional<std::set<std::string_view>> TransportCatalogue::ReturnBusesWithStop(std::string_view id ) const{
    std::set<std::string_view> rez;
    if (!stops_to_buses_.count(id)){
        if (stops_ptr_.count(id)){
            return rez;
        }
        return std::nullopt;
    } else {
        std::set<std::string_view> collect = stops_to_buses_.at(id);
             for (std::string_view bus: collect){
                rez.insert(bus);
             }
    }
    return rez;
}


double TransportCatalogue::CountDist(std::string_view name) const{
    double count=0;
    std::vector<std::string_view> list = bus_ptr_.at(name);
    for (size_t i = 0; i < list.size() - 1; i++){//идем по остановкам   
        if (stop_distance_.count({list.at(i), list.at(i + 1)})){ //создадим пару так как она есть
            count += stop_distance_.at({list.at(i), list.at(i + 1)});
        } else {
            auto s={list.at(i+1), list.at(i)};
            count += stop_distance_.at({list.at(i+1), list.at(i)});// или перевернем пару
        }
    }
    return count;
        
    }
    void TransportCatalogue::AddStopsDistance(std::pair<std::string_view,std::string_view>stops, int distance){
        size_t one = std::find(name_stops_.begin(),name_stops_.end(),stops.first)-name_stops_.begin();
        size_t two = std::find(name_stops_.begin(),name_stops_.end(),stops.second)-name_stops_.begin();
        if ( two>=name_stops_.size() ){
            name_stops_.push_back(std::string(stops.second));
            std::pair<std::string_view,std::string_view> dist(name_stops_[one],name_stops_.back());
            stop_distance_[dist] = distance;
        } else {
            std::pair<std::string_view,std::string_view> dist(name_stops_[one],name_stops_[two]);
            stop_distance_[dist] = distance;  
        }
    }


}