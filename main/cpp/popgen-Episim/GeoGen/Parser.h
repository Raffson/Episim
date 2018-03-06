#pragma once
//
// Created by robbe on 6/03/18.
//

#include <memory>
#include <map>
#include "popgen-Episim/GeoGen/City.h"
#include "boost/filesystem.hpp"

std::map<int, std::shared_ptr<City>> parse_cities(const boost::filesystem::path&);
