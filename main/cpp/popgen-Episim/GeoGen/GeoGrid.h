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

#include "util/ConfigInfo.h"
#include "util/RNManager.h"

#include "popgen-Episim/GeoGen/City.h"
#include "popgen-Episim/GeoGen/Community.h"
#include "popgen-Episim/GeoGen/Household.h"
#include "popgen-Episim/GeoGen/Parser.h"
#include "popgen-Episim/GeoGen/Utility.h"

#include "DesignByContract.h"

#include "trng/lcg64.hpp"
#include "trng/uniform_int_dist.hpp"
#include "trng/discrete_dist.hpp"
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>

using namespace std;

namespace geogen {

/// Enum that represent indexes of Fractals map. That map contains all fraction data of our
/// population
enum class Fractions{
    SCHOOLED,           // % of pop that is in school [6yrs, 18yrs),
    // [3, 12) -> elementary school, [12, 18) -> middle+highschool.
    ACTIVE,             // % of pop that is activly working.
    YOUNG_WORKERS,      // % of pop in [18yrs, 26yrs) that is working.
    OLD_WORKERS,        // % of pop in [26yrs, 65yrs) that is working.
    TODDLERS,           // % of pop that is in  [0yrs, 3yrs).
    OLDIES,             // % of pop that is in [65yrs, oldest_person].
    STUDENTS,           // % of pop in [18, 26) that is enrolled at a college/university.
    COMMUTING_STUDENTS, // % of pop in [18,26) that is enrolled at a college/universit and commutes.
    COMMUTING_WORKERS   // % of pop in [18,65) that works and commutes.
};

/// Enum that represent indexes of sizes map. That map contains all size data
/// of our communities
enum class Sizes{
    SCHOOLS,    // Average size of a school.
    COLLEGES,   // Average size of a college.
    COMMUNITIES, // Average size of a community
    WORKPLACES, // Average size of a workplace
    AVERAGE_CP, // Average size of a contactpool
    MAXLC       // Amount of largest cities (cities with a college)
};


/**
 * Class representing our GeoGrid;
 * Geogrid contains information about the cities. placing of contactpools
 * (shools, workplaces...) within those cities. Will be used by popgen and
 * the gui.
 */
class GeoGrid
{

public:
        /// Default constructor which does nothing.
        // TODO: Do we need this??
        // Raphael@everyone, well now we do...
        // to initialize the fractals/sizes maps so that people don't try to access those maps,
        // expecting to get a value of 0 out of it...
        // either that, or the GeoGrid-Constructor-Test must expect throws...
        // for now I will implement the default constructor real quick, we can still throw it away afterwards...
        GeoGrid();

        /// Takes a filepath to city_config file.
        /// @param config: a path to a gegogen config file. This file contains
        ///             things like name of the city data file, information about
        ///             the population...
        explicit GeoGrid(const boost::filesystem::path& config);

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
        double       GetFraction(Fractions type) const { return m_fract_map.at(type); }

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

private:
        /// Returns index of city with smallest population from 'lc'
        /// used by adjustLargestCities(lc, city)
        unsigned int FindSmallest(const vector<City*>& lc);

        /// Adjusts 'lc' iff 'city' has more people than the city with the smallest population in 'lc'
        /// used by generate_colleges()
        void AdjustLargestCities(vector<City*>& lc, City& city);

        /// Counts the total population in th GeoGrid based on the cities
        /// in map cities.
        //unsigned int CountTotalPop() const;

        /// Assigns the main fractions: schooled, worker1, worker2 & rest
        void GetMainFractions(const vector<vector<double>>& hhs);

private: // DO NOT DELETE! this seperates private members from private methods, improves readability...

        /// Effective map of all our fractals. Read the Fractions Enum what it will contain.
        map<Fractions, double> m_fract_map{};

        /// Effective map of all our sizes. Read the Sizes Enum what it will contain.
        map<Sizes, unsigned int> m_sizes_map{};

        /// Contains the model for the age distribition for households
        vector<vector<double>> m_household_age_distr{};

        /// Contains all cities for the GeoGrid
        map<unsigned int, City> m_cities{};

        /// Keep a map of all communities?
        /// -> will put everything in place in comments,
        ///     if we need it, just uncomment it...
        /// map<unsigned int, Community*> m_communities;

        /// doing the same for commuters
        /// Contains information about number of commuters from a city to a city
        /// map<unsigned int, <map<unsigned int, unsigned int> > > m_commuting;

        /// Total population of simulation area
        unsigned int m_total_pop{};

        unsigned int m_school_count{};
        vector<City*> m_cities_with_college{};
};

static stride::util::RNManager generator;

} // namespace geogen
