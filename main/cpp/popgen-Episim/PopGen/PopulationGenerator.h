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
#include <ctime>

namespace popgen {

// Raphael@everyone perhaps shorten the name of this class to PopGenerator?
class PopulationGenerator
{
public:
        explicit PopulationGenerator(geogen::GeoGrid&);

        /// Generates a random population according to fractions found in m_geogrid.
        void GeneratePopulation();

private:
        void GeneratePerson(const double& age, const unsigned int hid,
                            const unsigned int pcid, stride::Population& pop, geogen::City& city);

        void AssignToSchools();
        void AssignToColleges();
        void AssignToWorkplaces();
        void AssignToCommunity();
        void AssignAll();

        void                              GetCommunitiesOfRandomNearbyCity(const geogen::City& city,
                                                               const geogen::CommunityType& community_type,
                                                               std::vector<geogen::Community*>& result);
        void                              GetNearestCollege(const geogen::City& origin,
                                                            std::vector<geogen::Community*>& result);
        std::vector<stride::Person*>      GetSchoolAttendants(geogen::City& city);
        void                              GenerateHousehold(unsigned int size, geogen::City& city);
        std::vector<stride::ContactPool*> GetContactPoolsOfColleges();
        void                              InitializeHouseholdFractions();
        void                              InitializeCommutingFractions();
        void                              InitializeCityPopFractions();
        unsigned int                      GetRandomHouseholdSize();
        double                            GetRandomAge(unsigned int hhsize);
        geogen::City&                     GetRandomCity(const std::vector<unsigned int>& ids,
                                                        const std::vector<double>& fracs);
        stride::ContactPool*              GetRandomCommunityContactPool(const geogen::CommunityType& type,
                                                                        vector<geogen::Community*>& comms,
                                                                        const bool commuting = false);
        const bool                        IsWorkingCommuter();
        const bool                        IsStudentCommuter();
        const bool                        IsStudent();
        const bool                        IsActive();

        std::vector<stride::Person*>      GetActives(geogen::City&);
        geogen::City&                     GetRandomCommutingCity(geogen::City& city,
                                                                 const std::vector<unsigned int>& city_ids);

private:
        ///ID generator for creating persons, starting from 0 which in this case doesn't matter...
        static unsigned int m_id_generator;

        geogen::GeoGrid& m_geogrid;

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

        /// This member will map a city's ID to the population fraction relative to the total population
        // this could be made more efficient by splitting them up in 2 vectors,
        // removing the need to use boost::copy in GeneratePopulation...
        std::map<unsigned int, double> m_city_pop_fracs;
};

} // namespace popgen
