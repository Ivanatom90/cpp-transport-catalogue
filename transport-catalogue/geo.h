#pragma once

#include <cmath>
#include <svg.h>
#include <numeric>
#include <algorithm>

namespace transport_catalogue {
namespace detail {
namespace geo {

const int EARTH_RADIUS = 6371000;
const double PI = 3.1415926535;

struct Coordinates {
    Coordinates(double lat, double lon): latitude(lat), longitude(lon){}
    double latitude;
    double longitude;

    bool operator==(const Coordinates& other) const {
        return latitude == other.latitude && longitude == other.longitude;
    }
    bool operator!=(const Coordinates& other) const {
        return !(*this == other);
    }
};

double compute_distance(Coordinates start, Coordinates end);





}//end namespace geo
}//end namespace detail
}//end namespace transport_catalogue
