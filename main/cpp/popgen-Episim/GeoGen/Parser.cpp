//
// Created by robbe on 6/03/18.
//
#include <iostream>

#include "Parser.h"
#include "util/CSV.h"

using namespace std;

namespace geogen {
namespace parser{


    map<int, shared_ptr<City>> parse_cities(const boost::filesystem::path& filename){

        map<int, shared_ptr<City>> result;
        stride::util::CSV read_in(filename);

        for (auto &it : read_in) {

            unsigned int id = (unsigned int)(stoi(it.getValue(0)));
            unsigned int province = (unsigned int)(stoi(it.getValue(1)));
            unsigned int population = (unsigned int)(stoi(it.getValue(2)));
            double x_coord = stof(it.getValue(3));
            double y_coord = stof(it.getValue(4));
            double longitude = stof(it.getValue(5));
            double latitude = stof(it.getValue(6));
            string name = it.getValue(7);

            Coordinate coord(x_coord, y_coord, longitude, latitude);
            shared_ptr<City> cty(new City(id, province, population, coord, name));

            result[id] = cty;
        }

        return result;
    };

} // namespace parser
} // namespace geogen

