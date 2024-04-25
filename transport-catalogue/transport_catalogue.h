#pragma once

#include <deque>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <map>
#include "geo.h"
#include <iostream>
#include <memory>



namespace catalogue{

	struct StopSt{
		std::string name;
		double latitude;
		double longitude;
		bool is_point;

	};

	struct  InfoBus{
		size_t amount=0;
		size_t unique=0;
		double length=0;

		bool IsEmpty(){
			if( amount==0 && unique==0 && length==0){
				return true;
			}
			return false;
		}
	};




using namespace geo_math;



struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2> &pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

class TransportCatalogue {
	// Реализуйте класс самостоятельно
	
	std::deque<StopSt> stops_;// хранит структуры остановок
	std::deque<std::pair<std::string,std::vector<std::string>>> buses_;// храник пары назыание маршрута и вектор остановок
	std::unordered_map<std::string_view,StopSt*> stops_ptr_;// словарь ключ - остановка значение - указатель на структуру этой установки
	std::unordered_map<std::string_view,std::vector<std::string>*> bus_ptr_;// словарь ключ - название маршрута значение - указатель на вектор остановок этого марщрута
	std::unordered_map<std::string,std::set<std::string_view>> bases_for_stops_;// хранит коллекцию маршрутов на которых есть остановка - ключ
	std::unordered_map<std::pair<std::string_view,std::string_view>,  int, pair_hash> stop_distance_;// ключ- пара остановой( возможно понадобится хешер!!!) значение- дистанция


	public:
	
	void AddStop(const std::string& id, double lat, double lag, bool point_is);
	void AddStop(const std::string& id);
	void AddBus(const std::string& id, const std::vector<std::string_view>& stops);
	InfoBus CountStation(std::string_view id ) const;
	InfoBus ReturnBus(std::string_view id)const;
	void BusForStop(std::string_view id );
	std::set<std::string> ReturnStop(std::string_view id )const;
	void SetStopNeighbour(std::string_view center, std::vector<std::pair<std::string_view, int>> neighbour); 
	double CountDist(std::string_view name) const;
	

};
}