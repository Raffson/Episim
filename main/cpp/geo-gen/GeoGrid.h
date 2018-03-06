//
// Created by beau on 3/5/18.
//

#ifndef STRIDE_GEOGRID_H
#define STRIDE_GEOGRID_H

#include <vector>

#include "Community.h"
#include "Household.h"
#include "City.h"

using namespace std;

class GeoGrid {
public:
    GeoGrid() = default;

private:
    /// Contains schools, higher educations, workplaces, primary communities and secondary communities
    vector<Community> communities;

    /// Contains all households for the GeoGrid
    vector<Household> households;

    /// Contains all cities for the GeoGrid
    vector<City> cities;
};


#endif //STRIDE_GEOGRID_H
