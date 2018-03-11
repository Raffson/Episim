//
// Created by beau on 3/5/18.
//
#include "math.h"

#include <random>

#include "boost/property_tree/ptree.hpp"
#include <boost/property_tree/xml_parser.hpp>


#include "GeoGrid.h"
#include "popgen-Episim/GeoGen/Parser.h"
#include "popgen-Episim/GeoGen/CommunityTypes/School.h"



using namespace std;

namespace geogen {

    GeoGrid::GeoGrid(const boost::filesystem::path & config_file) {

        //Setting up property tree to parse xml config file
        boost::property_tree::ptree p_tree;
        boost::property_tree::read_xml(config_file.string(), p_tree);

        //reading the cities data file
        boost::filesystem::path base_path = "data/";
        string city_file = p_tree.get("popgen.data_files.cities","flanders_cities.csv");
        m_cities = parser::parse_cities(base_path.append(city_file));

        //Generating schools
        auto total_pop = p_tree.get<unsigned int>("popgen.pop_info.pop_total");
        //specs ask this to be read out of config, but could be calculated directly
        //out of the city file?
        auto schooled_fract = p_tree.get<float>("popgen.pop_info.fraction_schooled");
        auto school_size = p_tree.get<unsigned int>("popgen.contactpool_info.school.size");
        generate_schools(total_pop, schooled_fract, school_size);


    }

    void GeoGrid::generate_schools(const unsigned int pop_total, const float fract
            ,const unsigned int school_size) {

        // Calculating extra data
        // rounded because we don't have a fraction of a person
        auto amount_schooled = (const unsigned int) std::round(pop_total * fract);
        // round because we do not build half a school
        auto amount_of_schools = (const unsigned int) std::round(amount_schooled / school_size);


        // Setting up to divide the schools to cities
        std::vector<unsigned int> pop_id; //We will push the id's of the cities for each pop member.
        for(auto &it: m_cities){
            auto c_schooled_pop = (unsigned int) std::round(it.second->getPopulation() * fract);
            pop_id.insert(pop_id.end(), c_schooled_pop, (const unsigned int &) it.first);
        }
        //Note that this way cuz of rounding we lose a couple of schooled ppl.
        //But this shouldn't affect our city divison.


        random_device rand_dev;
        mt19937 gen (rand_dev()); //TODO need to remember state of this prob should be selectable.
        uniform_int_distribution<int>  distr(0, pop_id.size() - 1);

        for(unsigned int i = 0; i < amount_of_schools; i++){

            shared_ptr<School> nw_school(new School(school_size));
            int index = pop_id[distr(gen)];
            shared_ptr<City> chosen_city = m_cities[index];
            chosen_city->addSchool(nw_school);
        }


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


    unsigned int GeoGrid::count_total_pop() const {

        unsigned int counter = 0;
        for (auto &it : m_cities) {
            counter += it.second->getPopulation();
        }
        return counter;
    }

}//namespace geogen