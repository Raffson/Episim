#pragma once
//
// Created by Nishchal Shrestha on 20/03/2018.
//

#include "popgen-Episim/GeoGen/GeoGrid.h"
#include "popgen-Episim/GeoGen/Household.h"

#include <boost/assign.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>

#include "trng/discrete_dist.hpp"
#include "trng/uniform_dist.hpp"

#include <cmath>
#include <iterator>
#include <vector>

namespace popgen {

// Raphael@everyone perhaps shorten the name of this class to PopGenerator?
class PopulationGenerator
{
public:
        explicit PopulationGenerator(geogen::GeoGrid&, unsigned int rad = 10);

        ///
        void AssignHouseholds();
        void AssignToSchools();
        void AssignToColleges();
        void AssignToWorkplaces();
        void AssignToCommunity();
        void AssignAll();

        /*
         * @Returns the id of cities which lie in the radius of
         * @param radius km from
         * @param origin
         */
        std::vector<geogen::City*> GetCitiesWithinRadius(const geogen::City& origin,
                                                         unsigned int radius, unsigned int last);

private:
        std::vector<stride::ContactPool*> GetNearbyContactPools(const geogen::City& city, geogen::CommunityType);
        std::vector<stride::Person*>      GetSchoolAttendants(geogen::City& city);
        void                              GenerateHousehold(unsigned int size, geogen::City& city);
        std::vector<stride::ContactPool*> GetContactPoolsOfColleges();
        void                              InitializeHouseholdFractions();
        void                              InitializeCommutingFractions();
        unsigned int                      GetRandomHouseholdSize();
        double                            GetRandomAge(unsigned int hhsize);
        bool                              IsWorkingCommuter();
        //bool                              IsStudentCommuter();
        //bool                              IsStudent();
        bool                              IsActive();

        std::vector<stride::Person*>      GetActives(geogen::City&);
        geogen::City&                     GetRandomCommutingCity(geogen::City&, const std::vector<int>&);

private:
        ///ID generator for creating persons, starting from 0 which in this case doesn't matter...
        static unsigned int m_id_generator;

        geogen::GeoGrid& m_geogrid;

        const unsigned int m_initial_search_radius;

        /// This member will represent the chances for households with 1, 2, 3, 4,... members
        /// The first element is the chance a household has 1 member,
        /// The second elemenet is the chance that a household has 2 members, and so on...
        std::vector<double> m_household_size_fracs;

        /// This member will map a household's size onto a vector representing
        /// the fractions for the age-composition of that household
        std::map<unsigned int, std::vector<double>> m_household_comp_fracs;

        /// This member will keep a commuting distribution for each city...
        /// thus, cityID -> commuting distribution for the city with cityID
        std::map<unsigned int, std::vector<double>> m_commuting_fracs;
};

} // namespace popgen
