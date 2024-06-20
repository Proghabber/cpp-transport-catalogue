#include "transport_catalogue.h"

namespace catalogue{
    void TransportCatalogue::AddStop(const std::string& id ,geo_math::Coordinates point){
        auto name = std::find(name_stops_.begin(), name_stops_.end(), id);
        if (name !=name_stops_.end() ){
            data_bus::Stop stop(*name, point);
            stops_.push_back(stop); 
            stops_ptr_[*name] = &stops_.back();
        } else {
            name_stops_.push_back(id);
            data_bus::Stop stop(name_stops_.back(), point);
            stops_.push_back(stop); 
            stops_ptr_[name_stops_.back()] = &stops_.back();
        }       
    }

    void TransportCatalogue::AddBus(const std::string& bus, const std::vector<std::string_view>& stops, bool is_roundtrip){
        std::vector<std::string_view> filtered_stops_vector;   
        for (auto& stop : stops){
            if (stops_ptr_.count(stop)){
                size_t index = static_cast<size_t>(std::find(name_stops_.begin(), name_stops_.end(), stop) - name_stops_.begin());
                filtered_stops_vector.push_back(name_stops_[index]);     
            }    
        }
        name_bus_.push_back(bus);
        buses_[name_bus_.back()] = {name_bus_.back(), filtered_stops_vector, is_roundtrip};
        bus_ptr_[name_bus_.back()] = buses_[name_bus_.back()].stops;
        for (auto stop : bus_ptr_.at(bus)){
            stops_to_buses_[stop].insert(buses_[name_bus_.back()].name);
        }
    }

    data_bus::InfoBus TransportCatalogue::CountInfoBetweenStations(std::string_view bus ) const {
        int count = 0;
        size_t repeat_stop = 0 ;
        std::map<std::string_view, size_t> statistics_repeat;
        Coordinates from;
        Coordinates to;
        double distance_geo = 0;

        std::vector<std::string_view> rezult = buses_.at(bus).MakeFullBus();
        double distance_track = CountDist(rezult);
        size_t count_stops = rezult.size();
        for (auto stop : rezult ){
            ++statistics_repeat[stop];
            to=stops_ptr_.at(stop)->point;    
            if (!count){
                from=stops_ptr_.at(stop)->point;
            }
            if (statistics_repeat[stop] >1 ){
                repeat_stop++;
            }
            distance_geo += ComputeDistance(to, from);
            from = to;
            count++;
        }
        double curvature = distance_track/distance_geo;
        return {buses_.at(bus).name, count_stops, count_stops-repeat_stop, distance_geo, distance_track,
                curvature, buses_.at(bus).is_roundtrip};
    }

    data_bus::InfoBus TransportCatalogue::FindBus(std::string_view id) const {
        if (!bus_ptr_.count(id)){ 
            return data_bus::InfoBus();
        }  
        return CountInfoBetweenStations(id);
    }

    data_bus::InfoStop TransportCatalogue::FindStop(std::string_view id) const {
        if(!stops_ptr_.count(id)){
            return data_bus::InfoStop();
        }
        return ReturnBusesWithStop(id);
    }

    const std::unordered_map<std::string_view, data_bus::Bus>& TransportCatalogue::GetAllBus() const {
        return buses_;
    }

    const std::unordered_map<std::string_view, data_bus::Stop *>& TransportCatalogue::GetAllstops() const {
        return stops_ptr_;
    }

    data_bus::InfoStop TransportCatalogue::ReturnBusesWithStop(std::string_view id ) const {
        data_bus::InfoStop collect;
        collect.name = stops_ptr_.at(id)->name;
        if(!stops_to_buses_.count(id)){
            collect.name = stops_ptr_.at(id)->name;
            return collect;
        }
        collect.name = stops_ptr_.at(id)->name;
        collect.stops = stops_to_buses_.at(id);   
        return collect;
    }

    double TransportCatalogue::CountDist(std::vector<std::string_view> list_stop) const {
        double count = 0;
        for (size_t i = 0; i < list_stop.size() - 1; i++){//идем по остановкам   
            auto straight = ReturnStopsDistance(list_stop.at(i), list_stop.at(i + 1));//создадим пару так как она есть
            auto reverse  = ReturnStopsDistance(list_stop.at(i+1), list_stop.at(i));// или перевернем пару
            if (straight != std::nullopt){
                count += straight.value();
            } else {
                count += reverse.value();
            }
        }
        return count;    
    }

    void TransportCatalogue::AddStopsDistance(std::string_view stop_one, std::string_view stop_two, int distance){
        size_t one = static_cast<size_t>(std::find(name_stops_.begin(), name_stops_.end(), stop_one)-name_stops_.begin());
        size_t two = static_cast<size_t>(std::find(name_stops_.begin(), name_stops_.end(), stop_two)-name_stops_.begin());
        if (two>=name_stops_.size()){
            name_stops_.push_back(std::string(stop_two));
            std::pair<std::string_view,std::string_view> dist(name_stops_[one],name_stops_.back());
            stop_distance_[dist] = distance;
        } else {
            std::pair<std::string_view,std::string_view> dist(name_stops_[one],name_stops_[two]);
            stop_distance_[dist] = distance;  
        }
    }

    std::optional<int> TransportCatalogue::ReturnStopsDistance(std::string_view stop_one, std::string_view stop_two) const {
        std::pair<std::string_view, std::string_view> stops(stop_one, stop_two);
        if (stop_distance_.count(stops)){
            return {stop_distance_.at(stops)};
        }
        return std::nullopt;
    }
}