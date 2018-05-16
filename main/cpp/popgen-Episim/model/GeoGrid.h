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
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "pop/Population.h"
#include "util/ConfigInfo.h"
#include "util/FileSys.h"
#include "util/RNManager.h"
#include "util/TimeStamp.h"


#include "City.h"
#include "Community.h"
#include "Household.h"
#include "popgen-Episim/util/Parser.h"
#include "popgen-Episim/util/Utility.h"

#include "popgen-Episim/util/DesignByContract.h"

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
 * (shools, workplaces...) within those cities.
 * Will be used by stride, popgen and the gui.
 */
class GeoGrid
{

public:
        /// Default constructor which initializes some members to 0.
        /// @param info: A const reference to an RNManager's state.
        GeoGrid(const util::RNManager::Info& info = util::RNManager::Info());

        /// Specialized constructor which will immediately initialize and generate the GeoGrid.
        /// @param p_tree: A property-tree containing the necessary information.
        /// @param info: A const reference to an RNManager's state.
        GeoGrid(const boost::property_tree::ptree& p_tree, const util::RNManager::Info& info = util::RNManager::Info());

        /// Takes a filepath to city_config file and initializes the GeoGrid.
        /// @param config: a path to a gegogen config file. This file contains
        ///             things like name of the city data file, information about
        ///             the population...
        /// @param contactFile: A boolean value indicating if we're overriding the 'output_contact_file' value
        /// of the given configuration. This is specifically to prevent output during the tests.
        void Initialize(const boost::filesystem::path& config, const bool contactFile = false);

        /// Takes a filepath to city_config file and initializes the GeoGrid.
        /// @param p_tree: A property-tree containing the necessary information.
        void Initialize(const boost::property_tree::ptree& p_tree);

        /// Resets the entire GeoGrid.
        void Reset();

        /// Generates the schools, places them into the cities
        /// using a discrete distribution.
        /// Preconditions:\n
        ///     \li REQUIRE(m_initialized, "Must call GeoGrid::Initialize() before generation.");\n
        ///     \li REQUIRE(m_schooled_frac <= 1, "Schooled Fract is bigger then 1, not possible!");\n
        ///     \li REQUIRE(m_schooled_frac >= 0, "Schooled fract can't be negative");\n
        ///     \li REQUIRE(m_school_size >= 0, "The initial school size can't be negative");
        ///
        /// Postconditions:\n
        ///     \li ENSURE(Schools are placed in cities according to discrete distribution) -> enforced in test enviroment
        void GenerateSchools();

        /// Generates the colleges, places them into the cities
        /// using a discrete distribution.
        /// Preconditions:\n
        ///     \li REQUIRE(m_initialized, "Must call GeoGrid::Initialize() before generation.");\n
        ///     \li REQUIRE(m_student_frac >= 0, "Student fractal can't be negative");\n
        ///     \li REQUIRE(m_student_frac <= 1, "Student fractal can't be more then 100%");\n
        ///     \li REQUIRE(m_workers1_frac >= 0, "Worker fractal can't be negative");\n
        ///     \li REQUIRE(m_workers1_frac <= 1, "Worker fractal can't be more then 100%");
        ///
        /// Postconditions:\n
        ///     \li ENSURE(colleges are placed in x biggest cities) -> enforced in test envirorement
        //          Need to enforce this ensure in the code as well...
        void GenerateColleges();

        /// Generates the workplaces, places them into the cities a random with chances
        /// discretly distributed according to size. Generates contactpools for the communities.
        /// Preconditions:\n
        ///     \li REQUIRE(m_initialized, "Must call GeoGrid::Initialize() before generation.");\n
        void GenerateWorkplaces();

        /// Generates the communties, places them into the cities a random with chances
        /// discretly distributed according to size. Generates contactpools for the communities.
        /// Preconditions:\n
        ///     \li REQUIRE(m_initialized, "Must call GeoGrid::Initialize() before generation.");\n
        void GenerateCommunities();

        /// Calls all other generator functions.
        void GenerateAll();

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
        /// @retval <const map<unsigned int, vector<vector<double>>>&> Returns the households as a 2D vector,
        ///                             with every vector representing a household.
        const map<unsigned int, vector<vector<double>>>& GetModelHouseholds() const { return m_model_households; }

        /// Getter
        /// @retval <const vector<City*>>> Returns the Cities that contain a College.
        const vector<City*>& GetCitiesWithCollege() { return m_cities_with_college; }

        /// Getter
        /// @retval <Coordinate> Returns a coordinate representing the center of the grid
        ///                      deduced from all cities in the grid.
        Coordinate GetCenterOfGrid();

        //deprecated?
        // Assigns the age fractions to the provided vector
        //void GetAgeFractions(vector<double>& popfracs);

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
        /// @retval <const bool> Returns whether or not the GeoGrid is initialized.
        const bool IsInitialized() const { return m_initialized; }

        /// Getter
        /// @retval <const util::RNManager&> A const reference to the random number generator being used by GeoGrid.
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

        /// Write the entire GeoGrid to files. The configuration will be written along with the cities,
        /// household model, commuting model, contact pool system and population.
        void WriteToFile() const;

        /// Writes the current models to files. All cities, commuting data and households will be written.
        /// @param path The working directory in which the files should be written. Default is the path of execution.
        void WriteModelsToFiles(const string& path = "") const;

        /// Writes the current cities to a file.
        /// @param fname The name of the file to be written.
        void WriteCitiesToFile(const string& fname = "cities.csv") const;

        /// Writes the current commuting data to a file.
        /// @param fname The name of the file to be written.
        void WriteCommutingToFile(const string& fname = "commuting.csv") const;

        /// Writes the current model of households to a file.
        /// @param fname The name of the file to be written.
        void WriteModelHouseholdsToFile(const string& fname = "households.xml") const;

        /// Writes the current population to a file.
        /// @param fname The name of the file to be written.
        void WritePopToFile(const string& fname = "population.csv") const;

        /// Writes the current contact pool system to a file.
        /// @param fname The name of the file to be written.
        void WriteCommunitiesToFile(const string& fname = "communities.csv") const;

        /// Writes the current RNG's state to a file.
        /// @param fname The name of the file to be written.
        void WriteRNGstateToFile(const string& fname = "RNG-state.xml") const;

        /// Reads an RNG's state from a file.
        /// @param fname The name of the file to be read from.
        void ReadRNGstateFromFile(const string& fname = "RNG-state.xml");

private:
        /// Returns index of city with smallest population from 'lc'
        /// used by adjustLargestCities(lc, city)
        unsigned int FindSmallest(const vector<City*>& lc);

        /// Adjusts 'lc' iff 'city' has more people than the city with the smallest population in 'lc'
        /// used by generate_colleges()
        void AdjustLargestCities(vector<City*>& lc, City& city);

        /// Assigns the main fractions: schooled, worker1, worker2 & rest
        void GetMainFractions();

        /// Computes for each city the distances to all other cities and classifies them
        /// in exponential order, assigning this to m_neighbours_in_radius. The default initial search radius = 10km.
        void ClassifyNeighbours();
        void ClassifyNeighbours2(); //for benchmarking purposes...

        /// Used by GeoGrid::Initialize to create an output-directory if needed.
        void InitOutputStuff();

        /// Used by GeoGrid::Initialize to add the Popgen's configuration ptree.
        void AddPopgenPtree();

        /// Used by GeoGrid::Initialize to read the Fractions and Sizes.
        void ReadFractionsAndSizes();

        /// Used by GeoGrid::Initialize to read cities, households & commuting data from files.
        void ReadDataFiles();

        /// Used by GeoGrid::Initialize to ensure correct fractions and a "correct" average contactpool size.
        /// With "correct" we assume the average contactpool's size is bigger than 0 and is capped by 1000.
        void EnforceEnsures();

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

        ///< A variable indicating if the GeoGrid was initialized.
        bool m_initialized;

        ///< The random number generator.
        util::RNManager m_rng;

        ///< Indicates whether or not we want to use random age generation.
        bool m_random_ages;

        ///< A boost R-tree used for neighbour classification
        bgi::rtree<rtElem, bgi::quadratic<16>> m_rtree;
};

} // namespace stride