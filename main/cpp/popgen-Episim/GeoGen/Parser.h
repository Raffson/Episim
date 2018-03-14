#pragma once
//
// Created by robbe on 6/03/18.
//

#include <memory>
#include <map>

#include "util/CSVRow.h"
#include "popgen-Episim/GeoGen/City.h"
#include "boost/filesystem.hpp"

using namespace std;

namespace geogen {
namespace parser {


    map<int, shared_ptr<City>> parse_cities(const boost::filesystem::path &);


} // namespace parser
} // namespace geogen
