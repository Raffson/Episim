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

void geogrid::GeoGrid::generate_colleges() {

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
