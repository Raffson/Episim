#pragma once
//
// Created by robbe on 6/03/18.
//

#include <iostream>
#include <memory>
#include <map>

#include "util/CSV.h"
#include "util/CSVRow.h"
#include "popgen-Episim/GeoGen/City.h"
#include "boost/filesystem.hpp"

using namespace std;

namespace geogen {
namespace parser {


    map<int, shared_ptr<City>> ParseCities(const boost::filesystem::path &,
        const boost::filesystem::path &, bool parse_commuting);
    /// @return map of pair of (origin_id, destination_id) as key and number of commuters as value
    void ParseCommuting(const boost::filesystem::path &, map<int, shared_ptr<City>> &);


} // namespace parser
} // namespace geogen
