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

void ParseCities(const boost::filesystem::path& cityFile, map<unsigned int, City>& cities, unsigned int& totalPop,
                 bgi::rtree<rtElem, bgi::quadratic<16>>& rtree)
{
        setlocale(LC_ALL, "C"); // Enforces . is the floating point delimitter (for STOD QT takes local machine delimmiter)
        util::CSV    readIn(cityFile);
        unsigned int counter = 0;
        totalPop            = 0;
        for (auto& it : readIn) {
                counter++;
                try {
                        auto id         = (const unsigned int)abs(stoi(it.GetValue("id")));
                        auto province   = (const unsigned int)abs(stoi(it.GetValue("province")));
                        auto population = (const unsigned int)abs(stoi(it.GetValue("population")));
                        auto xCoord    = stod(it.GetValue("x_coord"));
                        auto yCoord    = stod(it.GetValue("y_coord"));
                        auto longitude  = stod(it.GetValue("longitude"));
                        auto latitude   = stod(it.GetValue("latitude"));
                        auto name       = it.GetValue("name");
                        totalPop += population;

                        rtree.insert(make_pair(gPoint(longitude, latitude), id));
                        Coordinate coord(xCoord, yCoord, longitude, latitude);
                        cities.emplace(id, City(id, province, population, coord, name));
                } catch (exception& e) {
                        cout << e.what() << endl << "at row: " << counter << endl;
                }
        }
}

void ParseCommuting(const boost::filesystem::path& filename, map<unsigned int, City>& cities,
                    const map<Fractions, double>& fracs)
{

        util::CSV readIn(filename);

        std::vector<std::string> cityIds = readIn.getLabels();

        // removing id_ from the label
        for (auto& id : cityIds)
                id.erase(0, 3);

        // First calculate the total number of commuters so we can normalize on the fly...
        std::vector<double> totalCommuters(readIn.GetColumnCount(), 0);
        for (const auto& it : readIn)
                for (unsigned int i = 0; i < readIn.GetColumnCount(); i++)
                        totalCommuters[i] += it.GetValue<double>(i);

        double commfrac =
            (fracs.at(Fractions::MIDDLE_AGED) * fracs.at(Fractions::COMMUTING_WORKERS) +
             fracs.at(Fractions::YOUNG) * (1 - fracs.at(Fractions::STUDENTS)) *
                 fracs.at(Fractions::COMMUTING_WORKERS)) *
                fracs.at(Fractions::ACTIVE) +
            fracs.at(Fractions::YOUNG) * fracs.at(Fractions::STUDENTS) * fracs.at(Fractions::COMMUTING_STUDENTS);

        unsigned int index = 0;
        for (const auto& it : readIn) {
                // for each row should represent destinations with commuting numbers...
                // the rows are ordered the same way as the columns, which represent the origin...
                auto destinationId = (unsigned int)stoi(cityIds.at(index));

                // so when looping over a row's columns, we're looking the origins...
                for (unsigned int i = 0; i < readIn.GetColumnCount(); i++) {
                        auto commuters = it.GetValue<double>(i);

                        auto originId = (unsigned int)stoi(cityIds.at(i));

                        // normalize
                        double commutingPop = cities.at(originId).GetPopulation() * commfrac;
                        double normalized    = commutingPop * (commuters / totalCommuters[i]);

                        if (cities.count(destinationId)) {
                                cities.at(destinationId).SetInCommuters(originId, normalized);
                                cities.at(originId).SetOutCommuters(destinationId, normalized);
                        }
                }
                index++;
        }
}

map<unsigned int, vector<vector<double>>> ParseHouseholds(const boost::filesystem::path& path)
{
        boost::property_tree::ptree pTree;
        boost::property_tree::read_xml(path.string(), pTree);
        map<unsigned int, vector<vector<double>>> result;

        for (auto& node : pTree.get_child("HouseholdProfile")) {
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

        auto fragmentAmount = (unsigned int)rndm.GetGenerator(distribution)();

        for (unsigned int i = 0; i < fragmentAmount; i++) {

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
