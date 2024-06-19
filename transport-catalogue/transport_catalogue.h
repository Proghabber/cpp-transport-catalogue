#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <vector>
#include <set>
#include <unordered_map>
#include <map>
#include <optional>
#include <iostream>
#include <memory>
#include <algorithm>
#include "geo.h"
#include "domain.h"

namespace catalogue{
	using namespace geo_math;
	struct pair_hash
	{
		template <class T1, class T2>
		std::size_t operator() (const std::pair<T1, T2> &pair) const {
			return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
		}
	};

	class TransportCatalogue {
		std::deque<std::string> name_bus_;// имена маршрутов
		std::deque<std::string> name_stops_;// имена остановок
		std::deque<data_bus::Stop> stops_;// хранит структуры остановок
		std::unordered_map<std::string_view,data_bus::Bus> buses_;// хранит структуры маршрутов
		std::unordered_map<std::string_view,data_bus::Stop*> stops_ptr_;// словарь ключ - остановка значение - указатель на структуру этой установки
		std::unordered_map<std::string_view,std::vector<std::string_view>> bus_ptr_;// словарь ключ - название маршрута значение -  вектор остановок этого марщрута
		std::unordered_map<std::string_view,std::set<std::string_view>> stops_to_buses_;// хранит коллекцию маршрутов на которых есть остановка - ключ
		std::unordered_map<std::pair<std::string_view,std::string_view>,  int, pair_hash> stop_distance_;// ключ- пара остановой( возможно понадобится хешер!!!) значение- дистанция

		data_bus::InfoBus CountInfoBetweenStations(std::string_view bus) const;
		data_bus::InfoStop ReturnBusesWithStop(std::string_view id)const;
		double CountDist(std::vector<std::string_view> list_stop) const;
		std::optional<int> ReturnStopsDistance(std::string_view stop_one, std::string_view stop_two) const;

	public:
		void AddStop(const std::string& id,geo_math::Coordinates point);
		void AddBus(const std::string& bus, const std::vector<std::string_view>& stops, bool is_roundtrip);
		data_bus::InfoBus FindBus(std::string_view id) const;
		data_bus::InfoStop FindStop(std::string_view id) const;	
		void AddStopsDistance(std::string_view stop_one, std::string_view stop_two, int distance);
		const std::unordered_map<std::string_view,data_bus::Bus>& GetAllBus() const;
		const std::unordered_map<std::string_view,data_bus::Stop*>& GetAllstops() const;
	};
}