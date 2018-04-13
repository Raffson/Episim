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

        /// Generates a random population according to all relevant fractions.
        void GeneratePopulation();

private:
        /// Generates a person
        /// @param age Represents the age of the person, generated by GetRandomAge
        /// @param hid Represents the household's ID to which the person will belong
        /// @param pcid Represents the ID of the primary community to which this person will belong
        /// @param pop The population to which this person will be added
        /// @param city The city in which this person will live
        void GeneratePerson(const double& age, const unsigned int hid,
                            const unsigned int pcid, stride::Population& pop, geogen::City& city);

        /// Selects a random nearby city and puts the communities into 'result', this may be refactored so that
        /// the result is returned instead of given by reference. This is done by selecting a random city
        /// from the nearby cities found, followed by getting the communities of the given type and putting them into
        /// result.
        /// @param city The city for which we need to look for nearby communities.
        /// @param community_type The type of the communities we're looking for
        /// @param result A reference to the vector where we store the resulting communities.
        void                              GetCommunitiesOfRandomNearbyCity(const geogen::City& city,
                                                               const geogen::CommunityType& community_type,
                                                               std::vector<geogen::Community*>& result);

        /// Selects the nearest college relative to the position of 'origin'.
        /// @param origin The city for which we need to find the nearst college.
        /// @param result A reference the vector where we store the resulting colleges.
        void                              GetNearestCollege(const geogen::City& origin,
                                                            std::vector<geogen::Community*>& result);

        /// Generates a household of a given size for the given city.
        /// @param size The size of the household that will be generated, determined by GetRandomHouseholdSize.
        /// @param city A reference to the city in which the household will be added.
        void                              GenerateHousehold(unsigned int size, geogen::City& city);

        // deprecated?
        //std::vector<stride::ContactPool*> GetContactPoolsOfColleges();

        /// Initializes the distribution to be used for determining the random sizes for households, as well as
        /// determining the distribution used for determining the composition of the households.
        void                              InitializeHouseholdFractions();

        /// Initializes the distribution to be used for determining a random city for commuters.
        void                              InitializeCommutingFractions();

        /// Initializes the distribution to be used for determing a random city for each of the households.
        void                              InitializeCityPopFractions();

        /// Returns a random household size according to the distribution intialized in InitializeHouseholdFractions.
        /// @retval <unsigned int> A random size for a household.
        unsigned int                      GetRandomHouseholdSize();

        /// Returns a random age according to the household-composition.
        /// @param hhsize The size of the household which determines an age-distribution.
        /// @retval <double> A random age for a person.
        double                            GetRandomAge(unsigned int hhsize);

        /// Returns a reference to a random city according to the
        /// distribution initialized in InitializeCityPopFractions.
        /// @retval <geogen::City&> Returns a reference to a random city.
        geogen::City&                     GetRandomCity();

        /// Returns a pointer to a contact pool chosen at random. This is done by first choosing a random
        /// community from 'comms', followed by choosing a random contact pool from the chosen community.
        /// @param type The community type for which we want to find a random contact pool
        /// @param comms The communities from which we will choose 1 at random to then choose a random contact pool.
        /// @param commuting Indicates whether the person for which we're looking to find a random contact pool
        /// is a commuter.
        /// @retval <stride::ContactPool*>
        stride::ContactPool*              GetRandomCommunityContactPool(const geogen::CommunityType& type,
                                                                        vector<geogen::Community*>& comms,
                                                                        const bool commuting = false);

        /// A random function which will flip a coin to determine if a person is a working commuter.
        const bool                        IsWorkingCommuter();

        /// A random function which will flip a coin to determine if a person is a commuting student.
        const bool                        IsStudentCommuter();

        /// A random function which will flip a coin to determine if we're dealing with a student.
        const bool                        IsStudent();

        /// A random function which will flip a coin to determine if we're dealing with an active worker.
        const bool                        IsActive();

        /// Returns a  reference to a random city for a commuter according to the commuting distribution.
        /// @param city The city from which we assume the commuter is coming from.
        /// @retval <geogen::City&> The random city for the commuter.
        geogen::City&                     GetRandomCommutingCity(geogen::City& city);

private:
        ///ID generator for creating persons, starting from 0 which in this case doesn't matter...
        static unsigned int m_id_generator;

        /// Reference to the GeoGrid so we can access relevant information.
        geogen::GeoGrid& m_geogrid;

        /// This member will represent the chances for households with 1, 2, 3, 4,... members
        /// The first element is the chance a household has 1 member,
        /// The second element is the chance that a household has 2 members, and so on...
        std::vector<double> m_household_size_fracs;

        /// This member will map a household's size onto a vector representing
        /// the fractions for the age-composition of that household
        std::map<unsigned int, std::vector<double>> m_household_comp_fracs;

        /// This member will keep a commuting distribution for each city...
        /// thus, cityID -> commuting distribution for the city with cityID
        std::map<unsigned int, std::vector<double>> m_commuting_fracs;

        /// This member will keep the IDs for all cities to be used for getting a random city for a commuter.
        std::vector<unsigned int> m_city_ids;

        /// This member represents the population fraction relative to the total population. The i-th element
        /// corresponds to the distribution for the city with the i-th ID in m_city_ids.
        std::vector<double> m_city_pop_fracs;
};

} // namespace popgen
