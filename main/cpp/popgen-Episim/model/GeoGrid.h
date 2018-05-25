#pragma once
//
// Created by beau on 3/5/18.
//

#include "City.h"
#include "pop/Population.h"
#include "popgen-Episim/util/Parser.h"
#include "popgen-Episim/util/Utility.h"
#include "util/RNManager.h"

#include <map>
#include <memory>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>

using namespace std;

namespace stride {

/**
 * Class representing our GeoGrid;
 * Geogrid contains information about the cities. placing of contactpools
 * (shools, workplaces...) within those cities.
 * Will be used by stride, popgen and the gui.
 */
class GeoGrid
{

public:
        /// Resets the entire GeoGrid.
        //this should become "ReleasePopulation" or something like that...
        // aiming at being able to plug in/out a population...
        void Reset();

        /// Getter
        /// @retval <map<unsigned int, City>&> Returns the map of Cities in GeoGrid. Cities are
        ///                                       indexed according to their City ID.
        map<unsigned int, City>& GetCities() { return m_cities; }

        /// Getter
        /// @retval <unsigned int> returns the total population to be generated for GeoGrid.
        unsigned int GetTotalPop() const { return m_total_pop; }

        /// Getter
        /// @param type an enum (Fractions) value representing the fraction to be returned
        /// @retval <double> Returns the fraction correspronding to the given type.
        double GetFraction(Fractions type) const { return m_fract_map.at(type); }

        /// Getter
        /// @param type an enum (Sizes) value representing the average size to be returned
        /// @retval <unsigned int> Returns the average size correspronding to the given type.
        unsigned int GetAvgSize(Sizes type) const { return m_sizes_map.at(type); }

        /// Getter
        /// @retval <unsigned int> Returns the number of schools
        unsigned int GetSchoolCount() const { return m_school_count; }

        /// Retrieve a city by entering the id of the city in [].
        City& operator[](unsigned int i) { return m_cities.at(i); }

        /// Getter
        /// @retval <const map<unsigned int, vector<vector<double>>>&> Returns a map which takes
        /// a size and maps this to the households of that size as a 2D vector,
        /// with every vector in the 2D vector representing a household.
        const map<unsigned int, vector<vector<double>>>& GetModelHouseholds() const { return m_model_households; }

        /// Getter
        /// @retval <const vector<City*>>> Returns the Cities that contain a College.
        const vector<City*>& GetCitiesWithCollege() { return m_cities_with_college; }

        /// Getter
        /// @retval <Coordinate> Returns a coordinate representing the center of the grid
        ///                      deduced from all cities in the grid.
        Coordinate GetCenterOfGrid();

        /// Getter
        /// @retval <Population> Returns a reference to population
        shared_ptr<Population> GetPopulation() { return m_population; }

        /// Splits up the X fract cities that have less then Y fract of the total population in fragmented
        /// Cities.
        /// @param X The first fraction.
        /// @param Y The second fraction.
        /// @param p_vec A vector representing a probability distribution.
        void DefragmentSmallestCities(double X, double Y, const vector<double>& p_vec);

        /// Getter
        /// @retval <unsigned int> Returns the initial search radius.
        unsigned int GetInitialSearchRadius() const { return m_initial_search_radius; }

        /// Getter
        /// @retval <const vector<City*>&> A const reference to a vector of pointers to cities within the given radius
        /// that contain the given CommunityType.
        /// @param origin A constant reference to the city for which we want to get other cities in the given radius.
        /// @param radius Radius around the origin in km. If the radius is not valid, the closest valid radius will
        /// be chosen. Valid values are 2^x * (initial search radius) where x is a positive integer. If the radius
        /// exceeds the maximum range between cities, the largest possible radius is chosen.
        /// @param type The type of the communities we're looking for.
        const vector<City*>& GetCitiesWithinRadiusWithCommunityType(const City& origin, unsigned int radius,
                                                                    CommunityType::Id type);

        /// Getter
        /// @retval <util::RNManager&> A const reference to the random number generator being used by GeoGrid.
        util::RNManager& GetRNG() { return m_rng; }

        /// Getter
        /// @retval <const bool> Returns whether or not we're using random ages for the population builder.
        const bool UsingRandomAges() const { return m_random_ages; }

        /// Getter
        /// @retval <unsigned int> Returns the total population of the model.
        unsigned int GetTotalPopOfModel() { return m_model_pop; } //used for tests

        /// Getter
        /// @retval <const boost::property_tree::ptree&> A const reference to the property tree
        /// representing the configuration.
        const boost::property_tree::ptree& GetConfigPtree() const { return m_config_pt; }

        /// Function to release the current population so that a new one can be generated.
        void ReleasePopulation();

private:
        friend class GeoGridGenerator;
        friend class GeoGridFileWriter;

        /// Default constructor for friends only.
        GeoGrid();

private: // DO NOT DELETE! this seperates private members from private methods, improves readability...

        ///< The property tree containing the configuration.
        boost::property_tree::ptree m_config_pt;

        ///< Effective map of all our fractions. Read the Fractions Enum what it will contain.
        map<Fractions, double> m_fract_map{};

        ///< Effective map of all our sizes. Read the Sizes Enum what it will contain.
        map<Sizes, unsigned int> m_sizes_map{};

        ///< Contains the model for the for households. The key is the size of the household while the value contains
        ///< all households of that size. Each houshold is represented by a vector with the ages of the occupants.
        map<unsigned int, vector<vector<double>>> m_model_households{};

        ///< Contains all cities for the GeoGrid
        map<unsigned int, City> m_cities{};

        ///< Contains cityIDs that are mapped to a map which holds a vector with pointers to cities
        ///< for each of the radius distance-categories (10km, 20km, 40km, ...) in relation to the city with ID
        map<unsigned int, map<unsigned int, map<CommunityType::Id, vector<City*>>>> m_neighbours_in_radius;

        ///< The initial search-radius for getting nearby cities
        unsigned int m_initial_search_radius;

        ///< Total population of simulation area
        unsigned int m_total_pop{};

        ///< Keeps count of the total population the model.
        unsigned int m_model_pop;

        ///< Total number of schools
        unsigned int m_school_count{};

        ///< Vector of pointer to all cities that have a college
        vector<City*> m_cities_with_college{};

        ///< The population of the GeoGrid
        shared_ptr<Population> m_population;

        ///< The random number generator.
        util::RNManager m_rng;

        ///< Indicates whether or not we want to use random age generation.
        bool m_random_ages;

        ///< A boost R-tree used for neighbour classification
        bgi::rtree<rtElem, bgi::quadratic<16>> m_rtree;
};

} // namespace stride
