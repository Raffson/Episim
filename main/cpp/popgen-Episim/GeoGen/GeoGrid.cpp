//
// Created by beau on 3/5/18.
//

#include "GeoGrid.h"
#include "popgen-Episim/GeoGen/Parser.h"

#include <cmath>
#include <popgen-Episim/GeoGen/CommunityTypes/PrimSec.h>

using namespace std;

namespace geogen {

    GeoGrid::GeoGrid(map<int, shared_ptr<City>> cities) : m_cities(move(cities)) {


        generate_all();
    }

    GeoGrid::GeoGrid(const boost::filesystem::path &map) :
            GeoGrid(parser::parse_cities(map)) {

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
        vector<shared_ptr<PrimSec>> primsec_communities;
        /// Communities need to be distributed according to the relative population size.
        /// First we need to determine the total number of communities to be used.
        auto total_pop = count_total_pop();
        auto total_communities = ceil(total_pop/2000);
        for (auto it : m_cities){
            shared_ptr<City> city = it.second;
            auto ratio = city->getPopulation()/total_pop;
            /// Now we have the ratio, we know that the city has ratio % of all communities.
            auto city_communities = (total_communities*ratio)/100;
            for (int i = 0; i < city_communities; i++){
                shared_ptr<PrimSec> primsec = make_shared<PrimSec>();
                primsec->setCity(city);
                primsec_communities.push_back(primsec);
            }
        }
        /// TODO: determine if community is primary or secundary.
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