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

#include "DesignByContract.h"


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

        GeoGrid() = default;


        /// Takes a filepath to city_config file.
        /// @param map: a path to a gegogen config file. This file contains
        ///             things like name of the city data file, information about
        ///             the population...
        explicit GeoGrid(const boost::filesystem::path& config);

        /// Generates the schools.
        void GenerateSchools();

        void GenerateColleges();

        void GenerateWorkplaces();

        void GenerateCommunities();

        void GenerateAll();

        const map<int, shared_ptr<City>>& get_cities();

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

        shared_ptr<City>& operator[](int i);

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



    private:

        /// Contains all households for the GeoGrid -> perhaps move this into City?
        vector<Household> m_households;

        /// Contains all cities for the GeoGrid
        map<int, shared_ptr<City>> m_cities;




        /// Keep a map of all communities?
        /// -> will put everything in place in comments,
        ///     if we need it, just uncomment it...
        ///map<unsigned int, shared_ptr<Community>> m_communities;


        ///doing the same for commuters
        /// Contains information about number of commuters from a city to a city
        ///map<unsigned int, <map<unsigned int, unsigned int> > > m_commuting;


        /// Total population of simulation area -> make this const?
        unsigned int m_total_pop;

        /// Fraction of population that goes to school -> make this const?
        float m_schooled_frac;

        /// Fraction of population that are able to work between 18y and 26y -> make this const?
        float m_workers1_frac;

        /// Fraction of population that are able to work between 27y and 65y -> make this const?
        float m_workers2_frac;

        /// Fraction of population younger than 3y and older than 65y -> make this const?
        float m_rest_frac;

        /// Fraction of workers1 that is student -> make this const?
        float m_student_frac;

        ///the ratio of commuters that are workers -> make this const?
        float m_commuting_students_frac;

        ///Total population that is actually working -> make this const?
        /// for workers1 (18y-26y) mind that we first need to exclude the students...
        float m_active_frac;

        ///the ratio of commuters that are workers -> make this const?
        float m_commuting_workers_frac;


        /// Average size of each school -> make this const?
        unsigned int m_school_size;

        /// Average size of each college -> make this const?
        unsigned int m_college_size;

        /// Maximum nr of "largest cities" -> make this const?
        unsigned int m_maxlc;

        /// Primary/Secundary community limit -> make this const?
        unsigned int m_community_size;

        /// Average size of each workplaces -> make this const?
        unsigned int m_worksplace_size;

        ///making these members const requires reworking the constructor,
        /// or hack our way around the initialisation...

        unsigned int m_school_count;


    };

}//namespace geogen

