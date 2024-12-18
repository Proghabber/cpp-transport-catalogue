#pragma once

#include <cmath>

namespace geo_math{
    struct Coordinates {
        double lat;
        double lng;
        bool operator==(const Coordinates& other) const;
        bool operator!=(const Coordinates& other) const;
    };

    double ComputeDistance(Coordinates from, Coordinates to);
}