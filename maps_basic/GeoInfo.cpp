//
// Created by alina-mlynorik on 9/14/23.
//

#include <valarray>
#include "GeoInfo.h"

double GeoInfo::_kmIn1degLon(double lat) {
    double lonrad = lat * M_PI / 180.0;
    return cos(lonrad) * 40075.696 / 360;
}

double GeoInfo::km2lat(double dist) {
    return dist / KM_IN_1_DEG_LAT;
}

double GeoInfo::km2long(double dist, double point_lat) {
    return dist / _kmIn1degLon(point_lat);
}
