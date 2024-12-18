#define _USE_MATH_DEFINES
#include <cmath>

#include "geo.h"

namespace geo_math{
    bool  Coordinates::operator==(const Coordinates& other) const {
            return lat == other.lat && lng == other.lng;
    }

    bool Coordinates::operator!=(const Coordinates& other) const {
            return !(*this == other);
    }

    double ComputeDistance(Coordinates from, Coordinates to) {
        using namespace std;
        const int earth_radius = 6371000;
        const int right_angle = 180;
        const double pi = 3.1415926535;
        if (from == to) {
            return 0;
        }
        static const double dr = pi / right_angle;
        return acos(sin(from.lat * dr) * sin(to.lat * dr)
                    + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
            * earth_radius;
    }

}  // namespace geo