//
// Created by robbe on 29.05.18.
//
#include "popgen-Episim/generator/GeoGridGenerator.h"
#include "popgen-Episim/generator/PopulationGenerator.h"

int main(int argc, char** argv)
{
    for(int i = 0; i < 1000; i++ ){
        shared_ptr<stride::GeoGrid> grid = stride::GeoGridGenerator().Generate("run_default.xml", true);
        stride::PopulationGenerator(*grid).Generate();
    }
}
