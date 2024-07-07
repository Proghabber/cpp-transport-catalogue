#pragma once

#include <variant>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <string_view>
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

	using BusMap = std::map<std::string_view, data_bus::Bus>;
    using StopMap = std::map<std::string_view, data_bus::Stop *>;
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
		std::string type;
		std::string name;
    };

    struct BusCollect{
		std::vector<std::string_view> stops;
		std::vector<geo_math::Coordinates> cordinates;
		int color_number = 0;
    }; 

	using AllInfo = std::variant<std::monostate, std::pair<data_bus::InfoBus, int>, std::pair<data_bus::InfoStop, int>, MapRequest>;
}
