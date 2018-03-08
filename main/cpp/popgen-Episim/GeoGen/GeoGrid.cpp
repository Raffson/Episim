//
// Created by beau on 3/5/18.
//

#include "GeoGrid.h"
#include "popgen-Episim/GeoGen/Parser.h"


geogrid::GeoGrid::GeoGrid( map<int, shared_ptr<geogrid::City>> cities) :cities(std::move(cities)){


    generate_all();
}


geogrid::GeoGrid::GeoGrid(const boost::filesystem::path &map):
        GeoGrid(geogrid::parser::parse_cities(map)) {

}

void geogrid::GeoGrid::generate_schools(const unsigned int pop_total) {

}

unsigned int findSmallest(const std::vector<shared_ptr<geogrid::City>>& lc) {
    unsigned int smallest = 0;
    for(unsigned int i = 1; i < lc.size(); i++) {
        if (lc[smallest]->getPopulation() > lc[i]->getPopulation()) smallest = i;
    }
    return smallest;
}

void adjustLargestCities(std::vector<shared_ptr<geogrid::City>>& lc, const shared_ptr<geogrid::City>& city) {
    if( lc.size() < 10 ) lc.push_back(city);
    else {
        unsigned int citpop = city->getPopulation();
        unsigned int smallest = findSmallest(lc);
        if( citpop > lc[smallest]->getPopulation() ) lc[smallest] = city;
    }
}

void geogrid::GeoGrid::generate_colleges() {
    //need 10 largest cities, largest determined by number of people in the city...
    std::vector<shared_ptr<geogrid::City>> lc;
    for(auto& it : cities) {
        adjustLargestCities(lc, it.second);
    }

    //just checking which cities we found...
    for(auto& it : lc) {
        std::cout << it->getId() << "   " << it->getPopulation() << "   " << it->getName() << std::endl;
    }

    //now we need to generate colleges for each of these cities,
    // each college should have room for about 3000 (=average) students...
}

void geogrid::GeoGrid::generate_workplaces() {

}

void geogrid::GeoGrid::generate_communities() {

}

void geogrid::GeoGrid::generate_all() {

    const unsigned int pop_total = count_total_pop();

    generate_schools(pop_total);
    generate_colleges();
    generate_workplaces();
    generate_communities();

}

unsigned int geogrid::GeoGrid::count_total_pop() const {

    unsigned int counter = 0;
    for(auto& it : cities){
        counter += it.second->getPopulation();
    }
    return counter;
}
