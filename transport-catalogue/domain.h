#pragma once

#include <variant>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <string_view>
#include <unordered_map>
#include "geo.h"

namespace data_bus{
	struct Stop{
		Stop(std::string_view name_org, geo_math::Coordinates point_org) : name(name_org), point(point_org) 
		{
		}
		std::string_view name;
		geo_math::Coordinates point;
	};

	struct Bus{
		std::string_view name;
		std::vector<std::string_view> stops;
		bool is_roundtrip;
		std::vector<std::string_view> MakeFullBus() const;
	};

	struct InfoBus{
		std::string_view name;
		size_t amount = 0;
		size_t unique = 0;
		double length = 0;
		double distance = 0;
		double curvature = 0;
		bool is_roundtrip = false;
		bool IsEmpty();
	};

	struct InfoStop{
		std::string_view name;
		std::set<std::string_view> stops;
		bool IsEmpty();
	};

	struct pair_hash{
		template <class T1, class T2>
		std::size_t operator() (const std::pair<T1, T2> &pair) const {
			return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
		}
	};

	using BusMap = std::map<std::string_view, data_bus::Bus>;
    using StopMap = std::map<std::string_view, data_bus::Stop*>;
	using CollectBus = std::unordered_map<std::string_view, data_bus::Bus>;// хранит структуры маршрутов
	using CollectStops = std::unordered_map<std::string_view, data_bus::Stop*>;// словарь ключ - остановка значение - указатель на структуру этой установки
	using BusRout = std::unordered_map<std::string_view,std::vector<std::string_view>>;// словарь ключ - название маршрута значение -  вектор остановок этого марщрута
	using StopInBuses = std::unordered_map<std::string_view,std::set<std::string_view>>;// хранит коллекцию маршрутов на которых есть остановка - ключ
	using StopsDist = std::unordered_map<std::pair<std::string_view,std::string_view>, int, data_bus::pair_hash>; // ключ- пара остановой значение- дистанция
}

namespace data_handler{
	struct MapRequest{
      int id;
      bool answer;
    };

    struct BusRequest{
        std::string name;
        bool is_roundtrip;
        std::vector<std::string> stops;
    };

    struct StopRequest{
		std::string name;
		geo_math::Coordinates point;
		std::map<std::string, int> distance;
    };

    struct AllRequest{
		std::vector<BusRequest> buses;
		std::vector<StopRequest> stops;
    };

    struct RetRequest{
		int id;
		size_t index;
		std::string type;
		std::string name;
		std::string from;
		std::string to;
    };

	struct RoutAnswer{
		int id;
		bool full;
		double speed;
		double wait_time;
		double all_time_go;
		std::string massage;
		std::string type;
		std::string name;
		std::string from;
		std::string to;
		std::vector<std::string_view> stops;
		std::vector<std::string_view> buses;
		std::vector<double> time_go;
		std::vector<int> bus_stop_count;
	};


    struct BusCollect{
		std::vector<std::string_view> stops;
		std::vector<geo_math::Coordinates> cordinates;
		int color_number = 0;
    }; 

	


	using AllInfo = std::variant<std::monostate, std::pair<data_bus::InfoBus, int>, std::pair<data_bus::InfoStop, int>, MapRequest, RoutAnswer>;
}
