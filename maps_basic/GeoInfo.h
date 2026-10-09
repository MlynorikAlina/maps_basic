//
// Created by alina-mlynorik on 9/14/23.
//

#ifndef TILECOMBINER_GEOINFO_H
#define TILECOMBINER_GEOINFO_H


namespace GeoInfo {
    const double KM_IN_1_DEG_LAT = 40008.55 / 360;

    double _kmIn1degLon(double lat);

    double km2lat(double dist);

    double km2long(double dist, double point_lat);
};


#endif //TILECOMBINER_GEOINFO_H
