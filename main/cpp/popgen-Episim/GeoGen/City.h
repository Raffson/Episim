//
// Created by beau on 3/5/18.
//

#ifndef STRIDE_CITY_H
#define STRIDE_CITY_H

#include <string>
#include <utility>
#include <cstddef>

using namespace std;

class City {
public:
    City();

private:
    /// Id generator.
    static std::size_t g_id_generator;

    /// A unique ID of the city.
    std::size_t city_id;

    // province ?

    /// Population of the city.
    std::size_t population;

    /// (x, y) position of the city on a map.
    pair<double, double> coords;

    /// (longitude, latitude) position of the city on a map.
    pair<double, double> long_lat;

    /// Name of the city.
    string name;
};


#endif //STRIDE_CITY_H
