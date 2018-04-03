//
// Created by Nishchal Shrestha on 20/03/2018.
//

#ifndef STRIDE_POPULATIONGENERATOR_H
#define STRIDE_POPULATIONGENERATOR_H

#include "popgen-Episim/GeoGen/GeoGrid.h"
#include "popgen-Episim/GeoGen/Household.h"

#include <boost/assign.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>

#include "trng/discrete_dist.hpp"
#include "trng/lcg64.hpp"
#include "trng/uniform_int_dist.hpp"

#include <cmath>
#include <iterator>
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
        shared_ptr<Household>                             GenerateHousehold(unsigned int size);
        std::vector<std::shared_ptr<stride::ContactPool>> GetContactPoolsOfColleges();
        void                                              InitializeHouseholdSizeFractions();
        void                                              InitializeCommutingFractions();
        trng::discrete_dist::result_type GetRandomHouseholdSize();
        trng::uniform_int_dist::result_type GetRandomAge();
        bool                                              IsWorkingCommuter();
        //bool                                              IsStudentCommuter();
        //bool                                              IsStudent();
        bool                                              IsActive();

        std::vector<Person>           GetActives(const shared_ptr<geogen::City>&);
        std::shared_ptr<geogen::City> GetRandomCommutingCity(const geogen::City&, const std::vector<int>&);

private:
        geogen::GeoGrid& m_geogrid;

        const unsigned int m_initial_search_radius;

        /// This member will represent the chances for households with 1, 2, 3, 4,... members
        /// The first element is the chance a household has 1 member,
        /// The second elemenet is the chance that a household has 2 members, and so on...
        std::vector<double> m_household_size_fracs;

        /// This member will keep a commuting distribution for each city...
        /// thus, cityID -> commuting distribution for the city with cityID
        std::map<unsigned int, std::vector<double>> m_commuting_fracs;
};

} // namespace popgen
#endif // STRIDE_POPULATIONGENERATOR_H
