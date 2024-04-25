#include "transport_catalogue.h"



namespace catalogue{
void TransportCatalogue::AddStop(const std::string& id ,double lat, double lag, bool point_is){
    stops_.push_back({id, lat, lag, point_is});
    stops_ptr_[stops_.back().name] = &stops_.back();
         
}

void TransportCatalogue::AddStop(const std::string& id ){
    AddStop(id,0,0,false);
         
}



void TransportCatalogue::AddBus(const std::string& id, const std::vector<std::string_view>& stops){
    std::vector<std::string> st;   
    for (auto& stop:stops){
        if (stops_ptr_.count(stop)){
            st.push_back(std::string(stop));     
        }    
    }
    buses_.push_back({id, std::move(st)});
    bus_ptr_[buses_.back().first] = &buses_.back().second;
    BusForStop(buses_.back().first);
}

void TransportCatalogue::BusForStop(std::string_view id){
     for (auto stop : *bus_ptr_.at(id)){
        bases_for_stops_[stop].insert(id);
     }
}


 InfoBus TransportCatalogue::CountStation(std::string_view id ) const{

    int count = 0;
    size_t amount = bus_ptr_.at(id)->size();
    size_t doubl =0 ;
    std::map<std::string, size_t> repeat;
    
    Coordinates from;
    Coordinates to;
    double distance = 0;

    for (auto s : *bus_ptr_.at(id) ){
        ++repeat[s];
        to={stops_ptr_.at(s)->latitude, stops_ptr_.at(s)->longitude};    
        if (!count){
            from={stops_ptr_.at(s)->latitude, stops_ptr_.at(s)->longitude};
        }
        if (repeat[s] >1 ){
             doubl++;
        }
        distance+=ComputeDistance(to, from);
        from = to;
        count++;
    }
   
    return {amount, amount-doubl, distance};
}

 InfoBus TransportCatalogue::ReturnBus(std::string_view id) const {
     InfoBus ret;
    if (!bus_ptr_.count(id)){ 
        return ret;
    }
    ret = CountStation(id);
    return ret;
}

std::set<std::string>  TransportCatalogue::ReturnStop(std::string_view id ) const{
    std::set<std::string> rez;
    if (!bases_for_stops_.count(std::string(id))){
        if (stops_ptr_.count(std::string(id))){
            rez.insert("no buses");
        } else {
            rez.insert("not found"); 
        }     
    } else {
        std::set<std::string_view> collect = bases_for_stops_.at(std::string(id));
             for (std::string_view bus: collect){
                rez.insert(std::string(bus));
             }
    }
    return rez;
}

void TransportCatalogue::SetStopNeighbour(std::string_view  center, std::vector<std::pair<std::string_view, int>> neighbour){
    for (auto n: neighbour){
        if ( stops_ptr_.count(n.first) ){
            stop_distance_[{stops_ptr_[center]->name,stops_ptr_[n.first]->name}]=n.second;
        } else {
            AddStop(std::string(n.first));        
            stop_distance_[{stops_ptr_[center]->name,stops_ptr_[n.first]->name}]=n.second;
        }
    }
        
}

double TransportCatalogue::CountDist(std::string_view name) const{
    double count=0;
    std::vector<std::string> list=*bus_ptr_.at(name);
    for (size_t i = 0; i < list.size() - 1; i++){//идем по остановкам   
        if (stop_distance_.count({list.at(i), list.at(i + 1)})){ //создадим пару так как она есть
            count += stop_distance_.at({list.at(i), list.at(i + 1)});
        } else {
            count += stop_distance_.at({list.at(i+1), list.at(i)});// или перевернем пару
        }
    }
    return count;
        
    }


}