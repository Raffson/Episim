//
// Created by robbe on 5/03/18.
//

#include <iostream>
#include <popgen-Episim/GeoGen/GeoGrid.h>

int main(int argc, char** argv)
{
    geogen::GeoGrid grid("config/geogen_default.xml");
    grid.generate_all();
}