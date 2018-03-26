//
// Created by Nishchal Shrestha on 20/03/2018.
//

#ifndef STRIDE_POPULATIONGENERATOR_H
#define STRIDE_POPULATIONGENERATOR_H


#include "popgen-Episim/GeoGen/GeoGrid.h"
#include "popgen-Episim/GeoGen/Household.h"

#include "boost/filesystem.hpp"

#include "trng/lcg64.hpp"
#include "trng/uniform_int_dist.hpp"

#include <cmath>
#include <vector>

namespace popgen{

//Raphael@everyone perhaps shorten the name of this class to PopGenerator?
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
    std::vector<std::shared_ptr<geogen::City>> GetCitiesWithinRadius(geogen::City origin, unsigned int radius);

private:
    // Raphael@Nishchal why? makes this kind of a god class...
    geogen::GeoGrid m_geogrid;

    // Raphael@Nishchal we should have only 1 generator, must be global for both geogrid and this class...
    trng::lcg64 m_generator;

    std::size_t m_id_generator;

};

}//namespace popgen
#endif //STRIDE_POPULATIONGENERATOR_H
