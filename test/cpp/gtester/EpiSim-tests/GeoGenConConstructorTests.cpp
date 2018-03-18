//
// Created by robbe on 15/03/18.
//
#include <iostream>
#include <gtest/gtest.h>

#include "popgen-Episim/GeoGen/GeoGrid.h"
#include "popgen-Episim/GeoGen/City.h"

using namespace std;

TEST(GeoGenTest, happy_day_constructor){

    ASSERT_NO_THROW(geogen::GeoGrid("config/geogen_default.xml"));


}

TEST(GeoGenTest, faulty_city_row){

    ASSERT_NO_THROW(geogen::parser::parse_cities("data/flanders_cities.csv"));
    map<int, shared_ptr<geogen::City>> cty_map = geogen::parser::parse_cities("data/flanders_cities_fualty_row.csv");
    EXPECT_EQ(326, cty_map.size());
}

TEST(GeoGenTest, faulty_city_col){

    ASSERT_NO_THROW(geogen::parser::parse_cities("data/flanders_cities.csv"));
    map<int, shared_ptr<geogen::City>> cty_map = geogen::parser::parse_cities("data/flanders_cities_faulty_col.csv");
    EXPECT_EQ(327, cty_map.size());

    shared_ptr<geogen::City> antwerp = cty_map[11002];
    geogen::Coordinate coord = antwerp->getCoordinates();

    EXPECT_EQ(1, antwerp->getProvince());
    EXPECT_EQ(150964, antwerp->getPopulation());
    EXPECT_EQ(153104.586, coord.x);
    EXPECT_EQ(212271.7101, coord.y);
    EXPECT_EQ(51.2165845, coord.latitude);
    EXPECT_EQ(4.413545489, coord.longitude);
    EXPECT_EQ("ANTWERPEN", antwerp->getName());

}

TEST(GeoGenTest,city_row_counter){

    geogen::GeoGrid grid("config/geogen_default.xml");
    EXPECT_EQ(327, grid.get_cities().size());

}

TEST(GeoGenTest,city_correct_data_test){

    geogen::GeoGrid grid("config/geogen_default.xml");
    map<int, shared_ptr<geogen::City>> mp = grid.get_cities();

    shared_ptr<geogen::City> antwerp = mp[11002];
    geogen::Coordinate coord = antwerp->getCoordinates();

    EXPECT_EQ(1, antwerp->getProvince());
    EXPECT_EQ(150964, antwerp->getPopulation());
    EXPECT_EQ(153104.586, coord.x);
    EXPECT_EQ(212271.7101, coord.y);
    EXPECT_EQ(51.2165845, coord.latitude);
    EXPECT_EQ(4.413545489, coord.longitude);
    EXPECT_EQ("Antwerpen", antwerp->getName());




}
