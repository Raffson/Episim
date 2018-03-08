//
// Created by robbe on 5/03/18.
//

#include <iostream>
#include "popgen-Episim/GeoGen/Parser.h"
int main(int argc, char** argv)
{
    geogen::parser::parse_cities("../../flanders_cities.csv");
    std::cout << "Hello World!" << std::endl;
}