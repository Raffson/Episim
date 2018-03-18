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
        unsigned  int counter = 0;
        for (auto &it : read_in) {
            counter++;
            try {
                unsigned int id = (unsigned int) (stoi(it.getValue("id")));
                unsigned int province = (unsigned int) (stoi(it.getValue("province")));
                unsigned int population = (unsigned int) (stoi(it.getValue("population")));
                string reader = it.getValue("x_coord");
                auto x_coord = stof(reader);
                float y_coord = stof(it.getValue("y_coord"));
                float longitude = stof(it.getValue("longitude"));
                float latitude = stof(it.getValue("latitude"));
                string name = it.getValue("name");


                Coordinate coord(x_coord, y_coord, longitude, latitude);
                shared_ptr<City> cty(new City(id, province, population, coord, name));

                result[id] = cty;
            }
            catch(exception& e){
                cout << e.what() << endl << "at row: " << counter << endl;
            }
        }

        return result;
    }

    map<pair<unsigned int, unsigned int>, unsigned int> parse_commuting(const boost::filesystem::path& filename){
        map<pair<unsigned int, unsigned int>, unsigned int> result;
        stride::util::CSV read_in(filename);

        std::vector<std::string> cityIds = read_in.getLabels();

        unsigned int index = 0;
        for(auto it: read_in){
            for(unsigned int i=0; i<cityIds.size(); i++){
                unsigned int commuters = (unsigned int)(stoi(it.getValue(cityIds.at(i))));
                string origin = cityIds.at(i);
                string destination = cityIds.at(index);
                unsigned int origin_id = (unsigned int) (stoi(origin.erase(0, 3)));
                unsigned int destination_id = (unsigned int) (stoi(destination.erase(0, 3)));

                pair<unsigned int, unsigned int> key = make_pair(origin_id, destination_id);
                result[key] = commuters;

            }
            index++;
        }

        return result;
    }


} // namespace parser
} // namespace geogen

