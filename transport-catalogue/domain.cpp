#include "domain.h"

namespace data_bus {

    std::vector<std::string_view> Bus::MakeFullBus() const{
			std::vector<std::string_view> rezult;
			if (!is_roundtrip){
				rezult.insert(rezult.end(), stops.begin(), stops.end() - 1);
				rezult.insert(rezult.end(), stops.rbegin(), stops.rend());
			} else {
				rezult.insert(rezult.end(), stops.begin(), stops.end());
			}
			return rezult;
		}

        bool InfoBus::IsEmpty(){
			if (amount == 0 && unique == 0 && length == 0 && length == 0 && distance == 0){
				return true;
			}
			return false;
		}

        bool InfoStop::IsEmpty()
		{
			if (!stops.size()){
				return true;
			}
			return false;
		}
}
