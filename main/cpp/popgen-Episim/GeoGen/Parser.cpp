//
// Created by robbe on 6/03/18.
//

#include "Parser.h"

using namespace std;

namespace stride {
namespace parser {

void ParseCities(const boost::filesystem::path& city_file, map<unsigned int, City>& cities, unsigned int& total_pop)
{
        util::CSV read_in(city_file);
        unsigned int counter = 0;
        total_pop = 0;
        for (auto& it : read_in) {
                counter++;
                try {
                        unsigned int id         = (unsigned int)(stoi(it.getValue("id")));
                        unsigned int province   = (unsigned int)(stoi(it.getValue("province")));
                        unsigned int population = (unsigned int)(stoi(it.getValue("population")));
                        double       x_coord    = stod(it.getValue("x_coord"));
                        double       y_coord    = stod(it.getValue("y_coord"));
                        double       longitude  = stod(it.getValue("longitude"));
                        double       latitude   = stod(it.getValue("latitude"));
                        string       name       = it.getValue("name");
                        total_pop += population;

                        Coordinate   coord(x_coord, y_coord, longitude, latitude);
                        cities.emplace(id, City(id, province, population, coord, name));
                } catch (exception& e) {
                        cout << e.what() << endl << "at row: " << counter << endl;
                }
        }
}

void ParseCommuting(const boost::filesystem::path& filename, map<unsigned int, City>& cities,
                    const map<Fractions, double>& fracs)
{

        util::CSV read_in(filename);

        std::vector<std::string> cityIds = read_in.getLabels();

        // removing id_ from the label
        for( auto& id : cityIds ) id.erase(0, 3);

        unsigned int index = 0;
        //First calculate the total number of commuters so we can normalize on the fly...
        std::vector<double> total_commuters(read_in.getColumnCount(), 0);
        for (auto it : read_in) {
            for (unsigned int i = 0; i < read_in.getColumnCount(); i++)
                total_commuters[i] += it.getValue<unsigned int>(i);
            index++;
        }

        double commfrac = fracs.at(Fractions::MIDDLE_AGED) * fracs.at(Fractions::COMMUTING_WORKERS)
                          + fracs.at(Fractions::YOUNG)
                            * (1 - fracs.at(Fractions::STUDENTS))
                            * fracs.at(Fractions::COMMUTING_WORKERS)
                          + fracs.at(Fractions::YOUNG)
                            * fracs.at(Fractions::STUDENTS)
                            * fracs.at(Fractions::COMMUTING_STUDENTS);

        index = 0;
        for (auto it : read_in) {
                // for each row should represent destinations with commuting numbers...
                // the rows are ordered the same way as the columns, which represent the origin...
                unsigned int destination_id = stoi(cityIds.at(index));

                // so when looping over a row's columns, we're looking the origins...
                for (unsigned int i = 0; i < read_in.getColumnCount(); i++) {
                        double commuters = it.getValue<unsigned int>(i);

                        unsigned int origin_id = stoi(cityIds.at(i));

                        // don't count local commuters...
                        if( origin_id == destination_id ) continue;

                        //normalize
                        double commuting_pop = cities.at(origin_id).GetPopulation() * commfrac;
                        double normalized = commuting_pop * (commuters / total_commuters[i]);

                        // we need a safety net here in case cities aren't read correctly..
                        // if city is present, we call SetInCommuters...
                        // else we should generate a warning, or maybe even throw an exception
                        // because this should NOT happen, litterly never...
                        if (cities.count(destination_id)) {
                                cities.at(destination_id).SetInCommuters(origin_id, normalized);
                                cities.at(origin_id).SetOutCommuters(destination_id, normalized);
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

vector<City> DefragmentCity(const City &city, vector<double> distr, util::RNManager &rndm) {

        trng::discrete_dist distribution(distr.begin(), distr.end());

        auto fragment_amount = (unsigned int)rndm.GetGenerator(distribution)();

        for(unsigned int i = 0; i < fragment_amount; i++){

                /* Putting stuff in comments to eliminate warnings...
                unsigned int id = city.GetId() * 10 + i; // TODO: a better way to distribute id's
                unsigned int province=  city.GetProvince(); //Stays the same
                unsigned int population = city.GetPopulation() / fragment_amount;
                *///TODO UNFINISHED
        }
        return vector<City>();
    }
} // namespace parser
} // namespace stride
