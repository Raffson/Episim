//
// Created by robbe on 6/03/18.
//
#include <iostream>

#include "Parser.h"
#include "util/CSV.h"

namespace geogrid {
namespace parser{


    std::map<int, std::shared_ptr<City>> parse_cities(const boost::filesystem::path& filename){

        std::map<int, std::shared_ptr<City>> result;
        stride::util::CSV read_in(filename);
        for (auto &it : read_in) {

            unsigned int id = std::stoi(it.getValue(0));
            unsigned int province = std::stoi(it.getValue(1));
            unsigned int population = std::stoi(it.getValue(2));
            double x_coord = std::stof(it.getValue(3));
            double y_coord = std::stof(it.getValue(4));
            double longitude = std::stof(it.getValue(5));
            double latitude = std::stof(it.getValue(6));
            string name = it.getValue(7);

            Coordinate coord(x_coord, y_coord, longitude, latitude);
            std::shared_ptr<City> cty(new City(id, province, population, coord, name));

            result[id] = cty;
        }

        return result;
    };

} // namespace parser
} // namespace geogen

