#pragma once
//
// Created by robbe on 6/03/18.
//

#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <algorithm>

#include "util/CSV.h"
#include "util/CSVRow.h"

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "popgen-Episim/GeoGen/City.h"
#include "popgen-Episim/GeoGen/Household.h"

using namespace std;

namespace geogen {
namespace parser {

map<int, shared_ptr<City>> ParseCities(const boost::filesystem::path&, const boost::filesystem::path&,
                                       bool parse_commuting);
/// @return map of pair of (origin_id, destination_id) as key and number of commuters as value
void ParseCommuting(const boost::filesystem::path&, map<int, shared_ptr<City>>&);

vector<vector<double>> ParseHouseholds(const boost::filesystem::path& path);

} // namespace parser
} // namespace geogen
