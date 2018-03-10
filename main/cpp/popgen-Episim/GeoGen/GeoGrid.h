#pragma once
//
// Created by beau on 3/5/18.
//


#include <vector>
#include <map>
#include <memory>
#include <iostream>

#include "popgen-Episim/GeoGen/CommunityTypes/Community.h"
#include "boost/filesystem.hpp"
#include "Household.h"
#include "City.h"

using namespace std;

namespace geogen {

    /*
     * Class representing our GeoGrid;
     * Geogrid contains information about the cities. placing of contactpools
     * (shools, workplaces...) within those cities. Will be used by popgen and
     * the gui.
     */
    class GeoGrid {

    public:

        GeoGrid() = default;


        //Takes a filepath to city_config file.
        //@param map: a path to a city file
        explicit GeoGrid(const boost::filesystem::path& map);

        // Generates the schools.
        void generate_schools(unsigned int pop_total);

        void generate_colleges();

        void generate_workplaces();

        void generate_communities();

        void generate_all();


    private:

        // Counts the total population in th GeoGrid based on the cities
        // in map cities.
        unsigned int count_total_pop() const;


    private:

        /// Contains schools, higher educations, workplaces, primary communities and secondary communities
        vector<Community> m_communities;

        /// Contains all households for the GeoGrid
        vector<Household> m_households;

        /// Contains all cities for the GeoGrid
        map<int, shared_ptr<City>> m_cities;

    };

}//namespace geogen

