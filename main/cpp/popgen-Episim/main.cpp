//
// Created by robbe on 5/03/18.
//

#include <iostream>
#include <popgen-Episim/GeoGen/GeoGrid.h>
#include "popgen-Episim/GeoGen/Parser.h"
int main(int argc, char** argv)
{
    geogrid::GeoGrid grid(geogrid::parser::parse_cities("data/flanders_cities.csv"));

}