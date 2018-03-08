//
// Created by beau on 3/5/18.
//

#include "GeoGrid.h"
#include "popgen-Episim/GeoGen/Parser.h"


geogrid::GeoGrid::GeoGrid( map<int, shared_ptr<geogrid::City>> cities) :cities(std::move(cities)){

    generate_all();
}


geogrid::GeoGrid::GeoGrid(const boost::filesystem::path &map):
        cities(geogrid::parser::parse_cities(map)) {

    generate_all();
}

void geogrid::GeoGrid::generate_schools() {

}

void geogrid::GeoGrid::generate_colleges() {

}

void geogrid::GeoGrid::generate_workplaces() {

}

void geogrid::GeoGrid::generate_communities() {

}

void geogrid::GeoGrid::generate_all() {

    generate_schools();
    generate_colleges();
    generate_workplaces();
    generate_communities();

}
