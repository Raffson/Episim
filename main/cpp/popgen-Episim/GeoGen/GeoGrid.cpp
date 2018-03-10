//
// Created by beau on 3/5/18.
//

#include "boost/property_tree/ptree.hpp"
#include <boost/property_tree/xml_parser.hpp>

#include "GeoGrid.h"
#include "popgen-Episim/GeoGen/Parser.h"



using namespace std;

namespace geogen {

    GeoGrid::GeoGrid(const boost::filesystem::path & config_file) {

        //Setting up property tree to parse xml config file
        boost::property_tree::ptree p_tree;
        boost::property_tree::read_xml(config_file.string(), p_tree);

        //reading the cities data file
        boost::filesystem::path base_path = "data/";
        string read_file = p_tree.get("popgen.data_files.cities","geogen_default.xml");
        m_cities = parser::parse_cities(base_path.append(read_file));

    }

    void GeoGrid::generate_schools(const unsigned int pop_total) {

    }

    unsigned int findSmallest(const vector <shared_ptr<City>> &lc) {
        unsigned int smallest = 0;
        for (unsigned int i = 1; i < lc.size(); i++) {
            if (lc[smallest]->getPopulation() > lc[i]->getPopulation()) smallest = i;
        }
        return smallest;
    }

    void adjustLargestCities(vector <shared_ptr<City>> &lc, const shared_ptr <City> &city) {
        if (lc.size() < 10) lc.push_back(city);
        else {
            unsigned int citpop = city->getPopulation();
            unsigned int smallest = findSmallest(lc);
            if (citpop > lc[smallest]->getPopulation()) lc[smallest] = city;
        }
    }

    void GeoGrid::generate_colleges() {
        //need 10 largest cities, largest determined by number of people in the city...
        vector <shared_ptr<City>> lc;
        for (auto &it : m_cities) {
            adjustLargestCities(lc, it.second);
        }

        //just checking which cities we found...
        for (auto &it : lc) {
            cout << it->getId() << "   " << it->getPopulation() << "   " << it->getName() << endl;
        }

        //now we need to generate colleges for each of these cities,
        // each college should have room for about 3000 (=average) students...
        // assigning students to colleges is not for now (yet),
        // so what do we need to do here, generate communities & add them to m_communities?
    }

    void GeoGrid::generate_workplaces() {

    }

    void GeoGrid::generate_communities() {

    }

    void GeoGrid::generate_all() {

        const unsigned int pop_total = count_total_pop();

        generate_schools(pop_total);
        generate_colleges();
        generate_workplaces();
        generate_communities();

    }

    unsigned int GeoGrid::count_total_pop() const {

        unsigned int counter = 0;
        for (auto &it : m_cities) {
            counter += it.second->getPopulation();
        }
        return counter;
    }

}//namespace geogen