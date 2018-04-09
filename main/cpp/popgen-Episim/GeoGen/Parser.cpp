//
// Created by robbe on 6/03/18.
//

#include "Parser.h"

using namespace std;

namespace geogen {
namespace parser {

void ParseCities(const boost::filesystem::path& city_file, const boost::filesystem::path& commuting_file,
                 map<unsigned int, City>& cities, bool parse_commuting)
{
        stride::util::CSV read_in(city_file);
        unsigned int      counter = 0;
        for (auto& it : read_in) {
                counter++;
                try {
                        unsigned int id         = (unsigned int)(stoi(it.getValue("id")));
                        unsigned int province   = (unsigned int)(stoi(it.getValue("province")));
                        unsigned int population = (unsigned int)(stoi(it.getValue("population")));
                        string       reader     = it.getValue("x_coord");
                        auto         x_coord    = stof(reader);
                        float        y_coord    = stof(it.getValue("y_coord"));
                        float        longitude  = stof(it.getValue("longitude"));
                        float        latitude   = stof(it.getValue("latitude"));
                        string       name       = it.getValue("name");

                        Coordinate   coord(x_coord, y_coord, longitude, latitude);
                        cities.emplace(id, City(id, province, population, coord, name));
                } catch (exception& e) {
                        cout << e.what() << endl << "at row: " << counter << endl;
                }
        }

        if (parse_commuting)
                ParseCommuting(commuting_file, cities);
}

void ParseCommuting(const boost::filesystem::path& filename, map<unsigned int, City>& cities)
{

        stride::util::CSV read_in(filename);

        std::vector<std::string> cityIds = read_in.getLabels();

        unsigned int index = 0;
        for (auto it : read_in) {
                unsigned int origin_id;

                for (unsigned int i = 0; i < cityIds.size(); i++) {
                        unsigned int commuters   = (unsigned int)(stoi(it.getValue(cityIds.at(i))));
                        string       origin      = cityIds.at(i);
                        string       destination = cityIds.at(index);

                        origin_id = (unsigned int)(stoi(origin.erase(0, 3))); // removing id_ from the label
                        unsigned int destination_id = (unsigned int)(stoi(destination.erase(0, 3)));

                        // we need a safety net here in case cities aren't read correctly..
                        // if city is present, we call SetInCommuters...
                        // else we should generate a warning, or maybe even throw an exception
                        // because this should NOT happen, litterly never...
                        if (cities.count(destination_id)) {
                                cities.at(destination_id).SetInCommuters(origin_id, commuters);
                                cities.at(origin_id).SetOutCommuters(destination_id, commuters);
                        }
                }
                index++;
        }
}

vector<vector<double>> ParseHouseholds(const boost::filesystem::path& path)
{
        boost::property_tree::ptree p_tree;
        boost::property_tree::read_xml(path.string(), p_tree);
        vector<vector<double>> result;

        for (auto& node : p_tree.get_child("HouseholdProfile")) {
                vector<double> ages;
                auto                 subtree = node.second;
                for (auto& v : subtree.get_child("")) {
                        double age = stod(v.second.data());
                        age = min(age, 80.0); //cause max age is 80 according to pop/Age.h
                        ages.push_back(age);
                }
                result.push_back(ages);
        }
        return result;
}
} // namespace parser
} // namespace geogen
