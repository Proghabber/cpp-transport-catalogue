#pragma once

#include <deque>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <map>
#include "geo.h"
#include <iostream>



namespace catalogue{

	struct StopSt{
		std::string name;
		double latitude;
		double longitude;

	};

	struct AllBussForStop{
		std::string reqest;
		std::set<std::string_view> collect;
	};
	

	struct BusCounted{
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

class TransportCatalogue {
	// Реализуйте класс самостоятельно
	
	std::deque<StopSt> stops_;//хранит структуры остановок
	std::deque<std::pair<std::string,std::vector<std::string>>> buses_;// храник пары назыание маршрута и вектор остановок
	std::unordered_map<std::string_view,StopSt*> stops_ptr_;// словарь ключ - остановка значение - указатель на структуру этой установки
	std::unordered_map<std::string_view,std::vector<std::string>*> bus_ptr_;// словарь ключ - название маршрута значение - указатель на вектор остановок этого марщрута
	std::unordered_map<std::string,std::set<std::string_view>> bases_for_stops_;  //хранит коллекцию маршрутов на которых есть остановка
	public:
	
	void AddStop(std::string id , double lat, double lag);
	void AddBus(std::string id, std::vector<std::string_view> stops);
	BusCounted CountStation(std::string_view id ) const;
	BusCounted ReturnBus(std::string_view id)const;
	void BusForStop(std::string_view id );
	AllBussForStop  ReturnStop(std::string_view id )const;

};
}