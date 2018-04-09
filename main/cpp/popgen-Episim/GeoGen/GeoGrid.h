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
        GeoGrid() = default;

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

        /// Returns the map of cities.
        const map<int, shared_ptr<City>>& GetCities();

        /// Bunch of getters, mainly for tests atm...
        /// Could leave all this out but then we need to work with friend classes...

        unsigned int GetTotalPop() const { return m_total_pop; }
        float        GetSchooledFrac() const { return m_fract_map.at(SCHOOLED); }
        float        GetWorkers1Frac() const { return m_fract_map.at(YOUNG_WORKERS); }
        float        GetWorkers2Frac() const { return m_fract_map.at(OLD_WORKERS); }
        float        GetToddlersFrac() const { return m_fract_map.at(TODDLERS); }
        float        GetOldiesFrac() const { return m_fract_map.at(OLDIES); }
        float        GetStudentFrac() const { return m_fract_map.at(STUDENTS); }
        float        GetCommutingStudentsFrac() const { return m_fract_map.at(COMMUTING_STUDENTS); }
        float        GetActiveFrac() const { return m_fract_map.at(ACTIVE); }
        float        GetCommutingWorkersFrac() const { return m_fract_map.at(COMMUTING_WORKERS); }
        unsigned int GetAvgCpSize() const { return m_sizes_map.at(AVERAGE_CP); }
        unsigned int GetSchoolSize() const { return m_sizes_map.at(SCHOOLS); }
        unsigned int GetCollegeSize() const { return m_sizes_map.at(COLLEGES); }
        unsigned int GetMaxLC() const { return m_sizes_map.at(MAXLC); }
        unsigned int GetCommunitySize() const { return m_sizes_map.at(COMMUNITES); }
        unsigned int GetWorkplaceSize() const { return m_sizes_map.at(WORKPLACES); }

        unsigned int GetSchoolCount() const;

        /// Retrieve a city by entering the id of the city in [].
        shared_ptr<City>& operator[](int i);

        /// Return the households of the geogrid
        const vector<vector<double>>& GetModelHouseholds() { return m_household_age_distr; }

        vector<shared_ptr<City>> GetCitiesWithCollege() { return m_cities_with_college; }

        /// Returns a coordinate representing the center of the grid deduced from all cities in the grid
        Coordinate GetCenterOfGrid();

private:
        /// Returns index of city with smallest population from 'lc'
        /// used by adjustLargestCities(lc, city)
        unsigned int FindSmallest(const vector<shared_ptr<City>>& lc);

        /// Adjusts 'lc' iff 'city' has more people than the city with the smallest population in 'lc'
        /// used by generate_colleges()
        void AdjustLargestCities(vector<shared_ptr<City>>& lc, const shared_ptr<City>& city);

        /// Counts the total population in th GeoGrid based on the cities
        /// in map cities.
        //unsigned int CountTotalPop() const;

        /// Assigns the main fractions: schooled, worker1, worker2 & rest
        void GetMainFractions(const vector<vector<double>>& hhs);

private: // DO NOT DELETE! this seperates private members from private methods, improves readability...

        /// Enum that represent indexes of Fractals map. That map contains all fraction data of our
        /// population
        enum Fractals{
            SCHOOLED,           // % of pop that is in school [6yrs, 18yrs),
                                // [6, 12) -> elementary school, [12, 18) -> middle+highschool.
            ACTIVE,             // % of pop that is activly working.
            YOUNG_WORKERS,      // % of pop in [18yrs, 26yrs) that is working.
            OLD_WORKERS,        // % of pop in [26yrs, 65yrs) that is working.
            TODDLERS,           // % of pop that is in  [0yrs, 6yrs).
            OLDIES,             // % of pop that is in [65yrs, oldest_person].
            STUDENTS,           // % of pop in [18, 26) that is enrolled at a college/university.
            COMMUTING_STUDENTS, // % of pop in [18,26) that is enrolled at a college/universit that commutes.
            COMMUTING_WORKERS   // % of pop in [18,65) that works that commutes.
        };
        /// Effective map of all our fractals. Read the Fractals Enum what it will contain.
        map<Fractals, float> m_fract_map{};

        /// Enum that represent indexes of sizes map. That map contains all size data
        /// of our communities
        enum Sizes{
            SCHOOLS,    // Average size of a school.
            COLLEGES,   // Average size of a college.
            COMMUNITES, // Average size of a community
            WORKPLACES, // Average size of a workplace
            AVERAGE_CP, // Average size of a contactpool
            MAXLC       // TODO: ??
        };

        /// Effective map of all our sizes. Read the Sizes Enum what it will contain.
        map<Sizes, unsigned int> m_sizes_map{};

        /// Contains the model for the age distribition for households
        vector<vector<double>> m_household_age_distr{};

        /// Contains all cities for the GeoGrid
        map<int, shared_ptr<City>> m_cities{};

        /// Keep a map of all communities?
        /// -> will put everything in place in comments,
        ///     if we need it, just uncomment it...
        /// map<unsigned int, shared_ptr<Community>> m_communities;

        /// doing the same for commuters
        /// Contains information about number of commuters from a city to a city
        /// map<unsigned int, <map<unsigned int, unsigned int> > > m_commuting;

        /// Total population of simulation area
        unsigned int m_total_pop{};
        unsigned int m_school_count{};
        vector<shared_ptr<City>> m_cities_with_college{};
        std::size_t m_id_generator{};


};

static stride::util::RNManager generator;

} // namespace geogen
