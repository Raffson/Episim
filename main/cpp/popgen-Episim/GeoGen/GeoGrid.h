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
        void generate_schools();

        void generate_colleges();

        void generate_workplaces();

        void generate_communities();

        void generate_all();

        map<int, shared_ptr<City>> get_cities();


    private:

        ///Returns index of city with smallest population from 'lc'
        /// used by adjustLargestCities(lc, city)
        unsigned int findSmallest(const vector <shared_ptr<City>> &lc);

        ///Adjusts 'lc' iff 'city' has more people than the city with the smallest population in 'lc'
        /// used by generate_colleges()
        void adjustLargestCities(vector <shared_ptr<City>> &lc, const shared_ptr <City> &city);

        /// Counts total in-commuters for a given destination
        unsigned int count_number_of_in_commuters(unsigned int destination_id);

        /// Counts total out-commuters for a given destination
        unsigned int count_number_of_out_commuters(unsigned int origin_id);

        /// Counts the total population in th GeoGrid based on the cities
        /// in map cities.
        unsigned int count_total_pop() const;

    private:

        /// Contains all households for the GeoGrid -> perhaps move this into City?
        vector<Household> m_households;

        /// Contains all cities for the GeoGrid
        map<int, shared_ptr<City>> m_cities;

        /// Contains information about number of commuters from a city to a city
        /// -> change this to map<unsigned int, map<unsigned int, unsigned int>>
        /// that'll make the usage afterwards way easier,
        /// e.g. m_communiting[city1.id][city2.id] gives the commuting info..
        map<pair<unsigned int, unsigned int>, unsigned int> m_commuting;

        /// Keep a map of all communities?
        /// -> will put everything in place in comments,
        ///     if we need it, just uncomment it...
        ///map<unsigned int, shared_ptr<Community>> m_communities;


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
        float m_commuting_students;

        ///Total population that is actually working -> make this const?
        /// for workers1 (18y-26y) mind that we first need to exclude the students...
        float m_active_frac;

        ///the ratio of commuters that are workers -> make this const?
        float m_commuting_workers;


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
        };

}//namespace geogen

