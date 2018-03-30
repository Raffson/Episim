//
// Created by Nishchal Shrestha on 20/03/2018.
//

#ifndef STRIDE_POPULATIONGENERATOR_H
#define STRIDE_POPULATIONGENERATOR_H

#include "popgen-Episim/GeoGen/GeoGrid.h"
#include "popgen-Episim/GeoGen/Household.h"

#include "boost/filesystem.hpp"

#include "trng/discrete_dist.hpp"
#include "trng/lcg64.hpp"
#include "trng/uniform_int_dist.hpp"

#include <cmath>
#include <vector>

namespace popgen {

// Raphael@everyone perhaps shorten the name of this class to PopGenerator?
class PopulationGenerator
{
public:
        PopulationGenerator(geogen::GeoGrid&, unsigned int rad = 10);

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
        std::vector<std::shared_ptr<geogen::City>> GetCitiesWithinRadius(const geogen::City& origin,
                                                                         unsigned int radius, unsigned int last);

private:
        std::vector<std::shared_ptr<stride::ContactPool>> GetNearbyContactPools(const geogen::City& city,
                                                                                geogen::CommunityType);
        std::vector<Person>                               GetSchoolAttendants(const shared_ptr<geogen::City>& city);
        std::shared_ptr<Household>                        GenerateHousehold(std::shared_ptr<Household> household);
        std::vector<std::shared_ptr<stride::ContactPool>> GetContactPoolsOfColleges();
        void                                              InitializeHouseholdSizeFractions();
        unsigned int                                      GetRandomHouseholdSize();
        unsigned int                                      GetRandomAge();
        bool                                              IsWorkingCommuter();
        bool                                              IsStudentCommuter();
        bool                                              IsStudent();
        bool                                              IsActive();

private:
        geogen::GeoGrid& m_geogrid;

        const unsigned int m_initial_search_radius;

        /// This member will represent the chances for households with 1, 2, 3, 4,... members
        /// The first element is the chance a household has 1 member,
        /// The second elemenet is the chance that a household has 2 members, and so on...
        std::vector<double> m_household_size_fracs;
};

} // namespace popgen
#endif // STRIDE_POPULATIONGENERATOR_H
