#pragma once
//
// Created by beau on 3/5/18.
//

#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "pop/Population.h"
#include "util/ConfigInfo.h"
#include "util/RNManager.h"

#include "popgen-Episim/GeoGen/City.h"
#include "popgen-Episim/GeoGen/Community.h"
#include "popgen-Episim/GeoGen/Household.h"
#include "popgen-Episim/GeoGen/Parser.h"
#include "popgen-Episim/GeoGen/Utility.h"

#include "DesignByContract.h"

#include "trng/discrete_dist.hpp"
#include "trng/lcg64.hpp"
#include "trng/uniform_int_dist.hpp"
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>

using namespace std;

namespace stride {

/**
 * Class representing our GeoGrid;
 * Geogrid contains information about the cities. placing of contactpools
 * (shools, workplaces...) within those cities. Will be used by popgen and
 * the gui.
 */
class GeoGrid
{

public:
        /// Default constructor which initializes some members to 0.
        GeoGrid();

        /// Takes a filepath to city_config file and initializes the GeoGrid.
        /// @param config: a path to a gegogen config file. This file contains
        ///             things like name of the city data file, information about
        ///             the population...
        void Initialize(const boost::filesystem::path& config);

        /// Resets the entire GeoGrid.
        void Reset();

        /// Generates the schools, places them into the cities
        /// using a discrete distribution.
        /// Preconditions:
        ///     REQUIRE(m_schooled_frac <= 1, "Schooled Fract is bigger then 1, not possible!");
        ///     REQUIRE(m_schooled_frac >= 0, "Schooled fract can't be negative");
        ///     REQUIRE(m_school_size >= 0, "The initial school size can't be negative");
        ///     ENSURE(Schools are placed in cities according to discrete distribution) -> enforced in test enviroment
        void GenerateSchools();

        /// Generates the colleges, places them into the cities
        /// using a discrete distribution.
        /// Preconditions:
        ///     REQUIRE(m_student_frac >= 0, "Student fractal can't be negative");
        ///     REQUIRE(m_student_frac <= 1, "Student fractal can't be more then 100%");
        ///     REQUIRE(m_workers1_frac >= 0, "Worker fractal can't be negative");
        ///     REQUIRE(m_workers1_frac <= 1, "Worker fractal can't be more then 100%");
        /// Postconditions:
        ///     ENSURE(colleges are placed in x biggest cities) -> enforced in test envirorement
        //          Need to enforce this ensure in the code as well...
        void GenerateColleges();

        /// Generates the workplaces, places them into the cities a random with chances
        /// discretly distributed according to size. Generates contactpools for the communities.
        void GenerateWorkplaces();

        /// Generates the communties, places them into the cities a random with chances
        /// discretly distributed according to size. Generates contactpools for the communities.
        void GenerateCommunities();

        /// Calls all other generator functions.
        void GenerateAll();

        /// Getter
        /// @retval <map<unsigned unt, City>&> returns the map of Cities in GeoGrid. Cities are
        ///                                       indexed according to their City ID.
        map<unsigned int, City>& GetCities() { return m_cities; }

        /// Getter
        /// @retval <unsigned int> returns the total population of GeoGrid.
        unsigned int GetTotalPop() const { return m_total_pop; }

        /// Getter
        /// @param type an enum (Fractions) value representing the fraction to be returned
        /// @retval <double> returns the fraction correspronding to the given type.
        double GetFraction(Fractions type) const { return m_fract_map.at(type); }

        /// Getter
        /// @param type an enum (Sizes) value representing the average size to be returned
        /// @retval <unsigned int> returns the average size correspronding to the given type.
        unsigned int GetAvgSize(Sizes type) const { return m_sizes_map.at(type); }

        /// Getter
        /// @retval <unsigned int> returns the number of schools
        unsigned int GetSchoolCount() const { return m_school_count; }

        /// Retrieve a city by entering the id of the city in [].
        City& operator[](unsigned int i) { return m_cities.at(i); }

        /// Getter
        /// @retval <const vector<vector<double>>&> returns the age distribution of our houses as a 2D vector.
        ///                             With every vector representing a household.
        const vector<vector<double>>& GetModelHouseholds() const { return m_household_age_distr; }

        /// Getter
        /// @retval <const vector<City*>>> Returns the Cities that contain a College.
        const vector<City*>& GetCitiesWithCollege() { return m_cities_with_college; }

        /// Getter
        /// @retval <Coordinate> Returns a coordinate representing the center of the grid
        ///                      deduced from all cities in the grid.
        Coordinate GetCenterOfGrid();

        /// Assigns the age fractions to the provided vector
        void GetAgeFractions(vector<double>& popfracs);

        /// Getter
        /// @retval <Population> Returns a reference to population
        Population& GetPopulation() { return m_population; }

        /// Getter
        /// @retval <boost::property_tree::ptree> Returns the property tree for Belief
        const boost::property_tree::ptree& GetBelief() const { return m_belief; }

        /// Splits up the X fract cities that have less then Y fract of the total population in fragmented
        /// Cities.
        /// @param X: a %.
        void DefragmentSmallestCities(double X, double Y, const vector<double>& p_vec);

        // don't think we need this next getter, just use GetCitiesWithinRadius...
        /// Getter
        /// @retval <map<unsigned int, map<unsigned int, vector<City*>>>> Returns a const reference to the
        /// map m_neighbours_in_radius which classifies all cities within a certain radius for each city.
        // const map<unsigned int, map<unsigned int, vector<City*>>>& GetNeighbourMap() { return m_neighbours_in_radius;
        // }

        /// Getter
        /// @retval <unsigned int> Returns the initial search radius.
        unsigned int GetInitialSearchRadius() { return m_initial_search_radius; }

        /// Getter
        /// @retval <const vector<City*>&> const reference to a vector of pointers to cities within the given radius.
        /// @param origin A constant reference to the city for which we want to get other cities in the given radius.
        /// @param radius Radius around the origin in km. If the radius is not valid, the closest valid radius will
        /// be chosen. Valid values are 2^x * (initial search radius) where x is a positive integer. If the radius
        /// exceeds the maximum range between cities, the largest possible radius is chosen.
        const vector<City*>& GetCitiesWithinRadius(const City& origin, unsigned int radius);

private:
        /// Returns index of city with smallest population from 'lc'
        /// used by adjustLargestCities(lc, city)
        unsigned int FindSmallest(const vector<City*>& lc);

        /// Adjusts 'lc' iff 'city' has more people than the city with the smallest population in 'lc'
        /// used by generate_colleges()
        void AdjustLargestCities(vector<City*>& lc, City& city);

        /// Assigns the main fractions: schooled, worker1, worker2 & rest
        void GetMainFractions(const vector<vector<double>>& hhs);

        /// Computes for each city the distances to all other cities and classifies them
        /// in exponential order, assigning this to m_neighbours_in_radius. The default initial search radius = 10km.
        void ClassifyNeighbours();

private: // DO NOT DELETE! this seperates private members from private methods, improves readability...
        /// Effective map of all our fractions. Read the Fractions Enum what it will contain.
        map<Fractions, double> m_fract_map{};

        /// Effective map of all our sizes. Read the Sizes Enum what it will contain.
        map<Sizes, unsigned int> m_sizes_map{};

        /// Contains the model for the age distribition for households
        vector<vector<double>> m_household_age_distr{};

        /// Contains all cities for the GeoGrid
        map<unsigned int, City> m_cities{};

        /// Contains cityIDs that are mapped to a map which holds a vector with pointers to cities
        /// for each of the radius distance-categories (10km, 20km, 40km, ...) in relation to the city with ID
        map<unsigned int, map<unsigned int, vector<City*>>> m_neighbours_in_radius;

        /// The initial search-radius for getting nearby cities
        unsigned int m_initial_search_radius;

        /// Keep a map of all communities?
        /// -> will put everything in place in comments,
        ///     if we need it, just uncomment it...
        /// map<unsigned int, Community*> m_communities;

        /// doing the same for commuters
        /// Contains information about number of commuters from a city to a city
        /// map<unsigned int, <map<unsigned int, unsigned int> > > m_commuting;

        /// Total population of simulation area
        unsigned int m_total_pop{};

        /// Keeps count of the total population the model.
        unsigned int m_model_pop;

        /// Total number of schools
        unsigned int m_school_count{};

        /// Vector of pointer to all cities that have a college
        vector<City*> m_cities_with_college{};

        /// The population of the GeoGrid
        Population m_population;

        /// Variable to store Belief used for creating people
        boost::property_tree::ptree m_belief;
};

} // namespace stride
