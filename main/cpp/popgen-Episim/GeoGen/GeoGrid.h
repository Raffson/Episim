//
// Created by beau on 3/5/18.
//

#ifndef STRIDE_GEOGRID_H
#define STRIDE_GEOGRID_H

#include <vector>
#include <map>
#include <memory>

#include "boost/filesystem.hpp"
#include "popgen-Episim/GeoGen/community_type/Community.h"
#include "Household.h"
#include "City.h"

using namespace std;

namespace geogrid {
    class GeoGrid {
    public:

        GeoGrid() = default;
        //constructor with a map of cities
        explicit GeoGrid( map<int, shared_ptr<City>> cities);

        /*
         * Takes a filepath to
         */
        explicit GeoGrid(const boost::filesystem::path& map);

        void generate_schools();

        void generate_colleges();

        void generate_workplaces();

        void generate_communities();

        void generate_all();


    private:
        /// Contains schools, higher educations, workplaces, primary communities and secondary communities
        vector<Community> communities;

        /// Contains all households for the GeoGrid
        vector<Household> households;

        /// Contains all cities for the GeoGrid
        map<int, shared_ptr<City>> cities;
    };
}//geogrid

#endif //STRIDE_GEOGRID_H
