//
// Created by beau on 3/5/18.
//

#include "City.h"

std::size_t City::g_id_generator = 1;

City::City()
{
    city_id = g_id_generator;
    g_id_generator++;
}