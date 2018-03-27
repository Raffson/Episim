#pragma once
//
// Created by beau on 3/5/18.
//


#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <cmath>
#include <random>

#include "boost/filesystem.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/xml_parser.hpp"

#include "popgen-Episim/GeoGen/Household.h"
#include "popgen-Episim/GeoGen/City.h"
#include "popgen-Episim/GeoGen/Parser.h"
#include "popgen-Episim/GeoGen/Community.h"
#include "popgen-Episim/GeoGen/Utility.h"

#include "DesignByContract.h"

#include "trng/lcg64.hpp"
#include "trng/uniform_int_dist.hpp"


using namespace std;

namespace geogen {

    /**
     * Class representing our GeoGrid;
     * Geogrid contains information about the cities. placing of contactpools
     * (shools, workplaces...) within those cities. Will be used by popgen and
     * the gui.
     */
    class GeoGrid {

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

        /// Generates the workplaces, places them into the cities
        /// using a discrete distribution.

        void GenerateWorkplaces();

        /// Generates the communties, places them into the cities
        /// using a discrete distribution.
        void GenerateCommunities();

        /// Calls all other generator functions.
        void GenerateAll();

        /// Returns the map of cities.
        const map<int, shared_ptr<City>>& GetCities();

        /// Bunch of getters, mainly for tests atm...
        /// Could leave all this out but then we need to work with friend classes...
        unsigned int GetTotalPop() const { return m_total_pop; }
        float GetSchooledFrac() const { return m_schooled_frac; }
        float GetWorkers1Frac() const { return m_workers1_frac; }
        float GetWorkers2Frac() const { return m_workers2_frac; }
        float GetRestFrac() const {return m_rest_frac; }
        float GetStudentFrac() const { return m_student_frac; }
        float GetCommutingStudentsFrac() const { return m_commuting_students_frac; }
        float GetActiveFrac() const { return m_active_frac; }
        float GetCommutingWorkersFrac() const { return m_commuting_workers_frac; }
        unsigned int GetSchoolSize() const { return m_school_size; }
        unsigned int GetCollegeSize() const { return m_college_size; }
        unsigned int GetMaxLC() const { return m_maxlc; }
        unsigned int GetCommunitySize() const { return m_community_size; }
        unsigned int GetWorkplaceSize() const { return m_worksplace_size; }

        unsigned int GetSchoolCount() const;

        /// Retrieve a city by entering the id of the city in [].
        shared_ptr<City>& operator[](int i);

        /// Return the households of the geogrid
        vector<Household> GetModelHouseholds(){ return m_model_households;}


        vector<shared_ptr<City>> GetCitiesWithCollege(){return m_cities_with_college;}

    private:

        ///Returns index of city with smallest population from 'lc'
        /// used by adjustLargestCities(lc, city)
        unsigned int FindSmallest(const vector<shared_ptr<City>> &lc);

        ///Adjusts 'lc' iff 'city' has more people than the city with the smallest population in 'lc'
        /// used by generate_colleges()
        void AdjustLargestCities(vector<shared_ptr<City>> &lc, const shared_ptr<City> &city);

        /// Counts the total population in th GeoGrid based on the cities
        /// in map cities.
        unsigned int CountTotalPop() const;


    private: //DO NOT DELETE! this seperates private members from private methods...

        /// Contains all households for the GeoGrid -> perhaps move this into City?
        // Raphael@Nishchal, if these households are already in the cities, the why do we need them here?
        //         and ffs, too much effort to make this a vector of shared pointers to households?

        //Nishchal@Rapahel These are models of households available in the xml file. there are only 26079 people
        //in that structure. What is done atm is when household is assigned to cities a copy from this model household
        // is taken and assigned to a city. That's why we have it here and in city
        //renamed to avoid confusion
        vector<Household> m_model_households{};

        /// Contains all cities for the GeoGrid
        map<int, shared_ptr<City>> m_cities{};


        /// Keep a map of all communities?
        /// -> will put everything in place in comments,
        ///     if we need it, just uncomment it...
        ///map<unsigned int, shared_ptr<Community>> m_communities;


        ///doing the same for commuters
        /// Contains information about number of commuters from a city to a city
        ///map<unsigned int, <map<unsigned int, unsigned int> > > m_commuting;


        /// Total population of simulation area
        unsigned int m_total_pop{};

        //According to the professor's mail:
        // [6,12) -> elementary school
        // [12, 18) -> middle+highschool
        // [18, 26) -> college/university
        // [18, 65) -> workers except those who go to college/university

        /// Fraction of population that goes to school (3y - 17y)
        float m_schooled_frac{};

        /*TODO: Next need to be constant -> re designing constructor othewise not possible.
        /(not that i know of)
        /Tought update: maybe users wants to 'experiment with those parameters in
        /the interface
        /Raphael@WhoeverWroteThisComment how? these are private...
        /and yes, once they are read from the config, this should remain constant...
        */
        /// Fraction of population that are able to work between 18y and 25y -> make this const?
        float m_workers1_frac{};

        /// Fraction of population that are able to work between 26y and 64y -> make this const?
        float m_workers2_frac{};

        /// Fraction of population younger than 3y and older than 64y -> make this const?
        float m_rest_frac{};

        /// Fraction of workers1 that is student -> make this const?
        float m_student_frac{};

        ///the ratio of commuters that are workers -> make this const?
        float m_commuting_students_frac{};

        ///Total population that is actually working -> make this const?
        /// for workers1 (18y-25y) mind that we first need to exclude the students...
        float m_active_frac{};

        ///the ratio of commuters that are workers -> make this const?
        float m_commuting_workers_frac{};


        /// Average size of each school -> make this const?
        unsigned int m_school_size{};

        /// Average size of each college -> make this const?
        unsigned int m_college_size{};

        /// Maximum nr of "largest cities" -> make this const?
        unsigned int m_maxlc{};

        /// Primary/Secundary community limit -> make this const?
        unsigned int m_community_size{};

        /// Average size of each workplaces -> make this const?
        unsigned int m_worksplace_size{};

        ///making these members const requires reworking the constructor,
        /// or hack our way around the initialisation...

        unsigned int m_school_count{};

        trng::lcg64 m_generator;

        vector<shared_ptr<City>> m_cities_with_college{};


    };

}//namespace geogen

