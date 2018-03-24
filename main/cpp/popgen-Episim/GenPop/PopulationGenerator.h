//
// Created by Nishchal Shrestha on 20/03/2018.
//

#ifndef STRIDE_POPULATIONGENERATOR_H
#define STRIDE_POPULATIONGENERATOR_H


#include "../GeoGen/GeoGrid.h"
#include "../GeoGen/Household.h"
#include "boost/filesystem.hpp"
#include "trng/lcg64.hpp"
#include <vector>

namespace genpop{

class PopulationGenerator {
public:

    PopulationGenerator(geogen::GeoGrid);

    void AssignHouseholds();
    void AssignToSchools();
    void AssignToColleges();
    void AssignToWorkplaces();
    void AssignToCommunity();
    void AssignAll();


    // TODO move this to an appropriate place
    /*
     * Based on haversine formula
     * credit: https://www.movable-type.co.uk/scripts/latlong.html
     * @return distance between two coordinates
     */
    double GetDistance(geogen::Coordinate c1, geogen::Coordinate c2);

    /*
     * @Returns the id of cities which lie in the radius of
     * @param radius km from
     * @param origin
     */
    std::vector<unsigned int> GetCitiesWithin(geogen::City origin, unsigned int radius);

private:
    geogen::GeoGrid m_geogrid;
    trng::lcg64 m_generator;
};

}//namespace genpop
#endif //STRIDE_POPULATIONGENERATOR_H
