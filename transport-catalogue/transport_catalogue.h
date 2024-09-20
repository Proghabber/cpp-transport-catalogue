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
	class TransportCatalogue {
		std::deque<std::string> name_bus_;// имена маршрутов
		std::deque<std::string> name_stops_;// имена остановок
		std::deque<data_bus::Stop> stops_;
		data_bus::CollectBus buses_;
		data_bus::CollectStops stops_ptr_;
		data_bus::BusRout bus_ptr_;
		data_bus::StopInBuses stops_to_buses_;
		data_bus::StopsDist stop_distance_;

		data_bus::InfoBus CountInfoBetweenStations(std::string_view bus) const;
		data_bus::InfoStop ReturnBusesWithStop(std::string_view id)const;
		double CountDist(std::vector<std::string_view> list_stop) const;
		std::optional<int> ReturnStopsDistance(std::string_view stop_one, std::string_view stop_two) const;
	public:
		void AddStop(const std::string& id, geo_math::Coordinates point);
		void AddBus(const std::string& bus, const std::vector<std::string_view>& stops, bool is_roundtrip);
		void AddStopsDistance(std::string_view stop_one, std::string_view stop_two, int distance);
		data_bus::InfoBus FindBus(std::string_view id) const;
		data_bus::InfoStop FindStop(std::string_view id) const;	
		const data_bus::CollectBus& GetAllBus() const;
		const data_bus::CollectStops& GetAllstops() const;
		int GetStopsDistanse(std::string_view one, std::string_view two) const;		
	};
}