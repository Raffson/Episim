#pragma once
//
// Created by Nishchal Shrestha on 20/03/2018.
//

#include "popgen-Episim/model/GeoGrid.h"
#include <vector>

namespace stride {

class Population;

class PopulationGenerator
{
public:
        explicit PopulationGenerator(GeoGrid&);

        /// Generates a random population according to the GeoGrid's model.
        void Generate();

private:
        /// Generates a person
        /// @param age Represents the age of the person, generated by GetRandomAge
        /// @param hid Represents the household's ID to which the person will belong
        /// @param scid Represents the ID of the secondary community to which this person will belong
        /// @param pop The population to which this person will be added
        /// @param city The city in which this person will live
        Person* GeneratePerson(const double& age, const size_t& hid, const size_t& scid, Population& pop, City& city);

        /// Selects a random nearby city and returns the communities of the given type.
        /// This is done by selecting a random city from the nearby cities found,
        /// followed by getting the communities of the given type and returning them.
        /// @param city The city for which we need to look for nearby communities.
        /// @param type The type of the communities we're looking for
        /// @retval <const vector<Community*>&> A const reference to the vector of pointers containing
        /// the resulting communities.
        const std::vector<Community*>& GetRandomCommunities(const City& city, const CommunityType::Id& type);

        /// Selects the nearest college relative to the position of 'origin'.
        /// @param origin The city for which we need to find the nearst college.
        /// @retval <vector<Community*>> The vector with the resulting colleges.
        std::vector<Community*> GetNearestColleges(const City& origin);

        /// Generates a household of a given size for the given city.
        /// @param size The size of the household that will be generated, determined by GetRandomHouseholdSize.
        /// @param city A reference to the city in which the household will be added.
        void GenerateHousehold(unsigned int size, City& city);

        /// Initializes the distribution to be used for determining the random sizes for households.
        void InitializeHouseholdFractions();

        /// Initializes the distribution to be used for determining a random city for commuters.
        void InitializeCommutingFractions();

        /// Initializes the distribution to be used for determing a random city for each of the households.
        void InitializeCityPopFractions();

        /// Returns a random household size according to the distribution intialized in InitializeHouseholdFractions.
        /// @retval <unsigned int> A random size for a household.
        unsigned int GetRandomHouseholdSize();

        /// Returns a random age according to the household-composition.
        /// @param category The age category for which we need to generate a random age.
        /// @retval <double> A random age for a person.
        double GetRandomAge(Fractions category);

        /// Returns a reference to a random city according to the
        /// distribution initialized in InitializeCityPopFractions.
        /// @retval <geogen::City&> Returns a reference to a random city.
        City& GetRandomCity();

        /// Returns a pointer to a contact pool chosen at random. This is done by first choosing a random
        /// community from 'comms', followed by choosing a random contact pool from the chosen community.
        /// @param comms The communities from which we will choose 1 at random to then choose 1 random contact pool.
        /// @retval <ContactPool*> The randomly selected contact pool.
        ContactPool* GetRandomContactPool(const vector<Community*>& comms);

        /// A random functions which flips an unfair coin, unless you pass 'frac' = 0.5
        /// @param frac The fraction of chance that you will get 'true' as a result.
        /// @retval <const bool> A contact boolean value indicating the result.
        bool FlipUnfairCoin(const double& frac);

        /// A random function which will flip a coin to determine if a person is a working commuter.
        bool IsWorkingCommuter();

        /// A random function which will flip a coin to determine if a person is a commuting student.
        bool IsStudentCommuter();

        /// A random function which will flip a coin to determine if we're dealing with a student.
        bool IsStudent();

        /// A random function which will flip a coin to determine if we're dealing with an active worker.
        bool IsActive();

        /// Returns a  reference to a random city for a commuter according to the commuting distribution.
        /// @param city The city from which we assume the commuter is coming from.
        /// @param student Indicates if we're looking for a city with a college for a commuting student.
        /// @retval <geogen::City&> The random city for the commuter.
        City& GetRandomCommutingCity(City& city, const bool student = false);

        /// Assigns a worker to a random contact pool.
        /// @param origin A reference to the city from which the worker is coming from.
        /// @retval <ContactPool*> A pointer to the contact pool to which the worker will be assigned.
        /// The assignment itself happens in GeneratePerson.
        ContactPool* AssignWorkerAtRandom(City& origin);

        /// Retrieves a random household of the given 'size' from GeoGrid's model.
        /// @param size The size of the household which we need.
        /// @retval <const std::vector<double>&> The randomly chosen household of the given size.
        const std::vector<double>& GetRandomModelHouseholdOfSize(unsigned int size);

private:
        ///< Reference to the GeoGrid for which we need to generate and link a population.
        GeoGrid& m_grid;

        ///< This member will represent the chances for households with 1, 2, 3, 4,... members
        ///< The first element is the chance a household has 1 member,
        ///< The second element is the chance that a household has 2 members, and so on...
        std::vector<double> m_household_size_fracs;

        ///> This member will keep a worker-commuting distribution for each city...
        ///< thus, cityID -> commuting distribution for the city with cityID
        std::map<unsigned int, std::vector<double>> m_worker_commuting_fracs;

        ///< This member will keep a student-commuting distribution for each city...
        ///< thus, cityID -> commuting distribution for the city with cityID
        std::map<unsigned int, std::vector<double>> m_student_commuting_fracs;

        ///< This member will keep the IDs for all cities to be used for getting a random city for assinging
        ///< a household as well as for a working commuter.
        std::vector<unsigned int> m_city_ids;

        ///< This member will keep the IDs for all cities with a college to be used for getting a random city for
        ///< a commuting student.
        std::vector<unsigned int> m_college_ids;

        ///< This member represents the population fraction relative to the total population. The i-th element
        ///< corresponds to the distribution for the city with the i-th ID in m_city_ids.
        std::vector<double> m_city_pop_fracs;

        ///< The random number generator.
        util::RNManager& m_rng;
};

} // namespace stride
