#pragma once

#include <deque>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <map>
#include <optional>
#include <iostream>
#include <memory>
#include <algorithm>
#include "geo.h"





namespace catalogue{

	struct StopSt{
		std::string_view name;
		geo_math::Coordinates point;
	};

	struct  InfoBus{
		size_t amount = 0;
		size_t unique = 0;
		double length = 0;
		double distance = 0;
		double curvature = 0;

		bool IsEmpty(){
			if( amount == 0 && unique == 0 && length == 0 && length == 0 && distance == 0){
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
	
	std::deque<std::string> name_bus_;// имена маршрутов
	std::deque<std::string> name_stops_;// имена остановок


	std::deque<StopSt> stops_;// хранит структуры остановок
	std::deque<std::pair<std::string_view,std::vector<std::string_view>>> buses_;// храник пары назыание маршрута и вектор остановок
	std::unordered_map<std::string_view,StopSt*> stops_ptr_;// словарь ключ - остановка значение - указатель на структуру этой установки
	std::unordered_map<std::string_view,std::vector<std::string_view>> bus_ptr_;// словарь ключ - название маршрута значение - указатель на вектор остановок этого марщрута
	std::unordered_map<std::string_view,std::set<std::string_view>> stops_to_buses_;// хранит коллекцию маршрутов на которых есть остановка - ключ
	std::unordered_map<std::pair<std::string_view,std::string_view>,  int, pair_hash> stop_distance_;// ключ- пара остановой( возможно понадобится хешер!!!) значение- дистанция

	InfoBus CountInfoBetweenStations(std::string_view bus ) const;
	double CountDist(std::string_view name) const;

	public:
	
	void AddStop(const std::string& id,geo_math::Coordinates point);
	void AddBus(const std::string& id, const std::vector<std::string_view>& stops);
	InfoBus FindBus(std::string_view id)const;
	void BusForStop(std::string_view id );
	std::optional<std::set<std::string_view>> ReturnBusesWithStop(std::string_view id )const;
	void AddStopsDistance(std::pair<std::string_view,std::string_view>stops, int distance);
	

};
}