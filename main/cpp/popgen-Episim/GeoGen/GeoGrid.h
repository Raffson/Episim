#pragma once
//
// Created by beau on 3/5/18.
//


#include <vector>
#include <map>
#include <memory>
#include <iostream>

#include "boost/filesystem.hpp"
#include "Household.h"
#include "City.h"

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

        void generate_colleges(unsigned int maxlc = 10);

        void generate_workplaces();

        void generate_communities();

        void generate_all();


    private:

        /// Counts the total population in th GeoGrid based on the cities
        /// in map cities.
        unsigned int count_total_pop() const;


    private:

        /// Contains all households for the GeoGrid -> perhaps move this into City?
        vector<Household> m_households;

        /// Contains all cities for the GeoGrid
        map<int, shared_ptr<City>> m_cities;

        /// Total population of simulation area
        unsigned int m_total_pop;

        /// Fraction of population that goes to school
        float m_schooled_frac;

        /// Average size of each school
        unsigned int m_school_size;

        /// Fraction of population that are students
        float m_student_frac;

        /// Average size of each college
        unsigned int m_college_size;

        /// Primary/Secundary community limit
        unsigned int m_community_size_limit;

    };

}//namespace geogen

