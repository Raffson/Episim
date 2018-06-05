//
// Created by robbe on 6/03/18.
//

#include "Parser.h"
#include "util/CSV.h"

#include "trng/discrete_dist.hpp"

#include <boost/filesystem.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <clocale>

using namespace std;

namespace stride {
namespace parser {

void ParseCities(const boost::filesystem::path& city_file, map<unsigned int, City>& cities, unsigned int& total_pop,
                 bgi::rtree<rtElem, bgi::quadratic<16>>& rtree)
{
        setlocale(LC_ALL, "C"); // Enforces . is the floating point delimitter (for STOD QT takes local machine delimmiter)
        util::CSV    read_in(city_file);
        unsigned int counter = 0;
        total_pop            = 0;
        for (auto& it : read_in) {
                counter++;
                try {
                        auto id         = (const unsigned int)abs(stoi(it.GetValue("id")));
                        auto province   = (const unsigned int)abs(stoi(it.GetValue("province")));
                        auto population = (const unsigned int)abs(stoi(it.GetValue("population")));
                        auto x_coord    = stod(it.GetValue("x_coord"));
                        auto y_coord    = stod(it.GetValue("y_coord"));
                        auto longitude  = stod(it.GetValue("longitude"));
                        auto latitude   = stod(it.GetValue("latitude"));
                        auto name       = it.GetValue("name");
                        total_pop += population;

                        rtree.insert(make_pair(gPoint(longitude, latitude), id));
                        Coordinate coord(x_coord, y_coord, longitude, latitude);
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
        for (auto& id : cityIds)
                id.erase(0, 3);

        // First calculate the total number of commuters so we can normalize on the fly...
        std::vector<double> total_commuters(read_in.GetColumnCount(), 0);
        for (const auto& it : read_in)
                for (unsigned int i = 0; i < read_in.GetColumnCount(); i++)
                        total_commuters[i] += it.GetValue<double>(i);

        double commfrac =
            (fracs.at(Fractions::MIDDLE_AGED) * fracs.at(Fractions::COMMUTING_WORKERS) +
             fracs.at(Fractions::YOUNG) * (1 - fracs.at(Fractions::STUDENTS)) *
                 fracs.at(Fractions::COMMUTING_WORKERS)) *
                fracs.at(Fractions::ACTIVE) +
            fracs.at(Fractions::YOUNG) * fracs.at(Fractions::STUDENTS) * fracs.at(Fractions::COMMUTING_STUDENTS);

        unsigned int index = 0;
        for (const auto& it : read_in) {
                // for each row should represent destinations with commuting numbers...
                // the rows are ordered the same way as the columns, which represent the origin...
                auto destination_id = (unsigned int)stoi(cityIds.at(index));

                // so when looping over a row's columns, we're looking the origins...
                for (unsigned int i = 0; i < read_in.GetColumnCount(); i++) {
                        double commuters = it.GetValue<double>(i);

                        auto origin_id = (unsigned int)stoi(cityIds.at(i));

                        // normalize
                        double commuting_pop = cities.at(origin_id).GetPopulation() * commfrac;
                        double normalized    = commuting_pop * (commuters / total_commuters[i]);

                        if (cities.count(destination_id)) {
                                cities.at(destination_id).SetInCommuters(origin_id, normalized);
                                cities.at(origin_id).SetOutCommuters(destination_id, normalized);
                        }
                }
                index++;
        }
}

map<unsigned int, vector<vector<double>>> ParseHouseholds(const boost::filesystem::path& path)
{
        boost::property_tree::ptree p_tree;
        boost::property_tree::read_xml(path.string(), p_tree);
        map<unsigned int, vector<vector<double>>> result;

        for (auto& node : p_tree.get_child("HouseholdProfile")) {
                vector<double> ages;
                auto           subtree = node.second;
                for (auto& v : subtree.get_child("")) {
                        double age = stod(v.second.data());
                        ages.push_back(age);
                }
                result[ages.size()].emplace_back(ages);
        }
        return result;
}

//What is this function???
vector<City> DefragmentCity(const City& city, vector<double> distr, util::RNManager& rndm)
{

        trng::discrete_dist distribution(distr.begin(), distr.end());

        auto fragment_amount = (unsigned int)rndm.GetGenerator(distribution)();

        for (unsigned int i = 0; i < fragment_amount; i++) {

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
