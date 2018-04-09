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

using namespace std;

namespace geogen {
namespace parser {

void ParseCities(const boost::filesystem::path&, const boost::filesystem::path&,
                 map<unsigned int, City>& cities, bool parse_commuting);

void ParseCommuting(const boost::filesystem::path&, map<unsigned int, City>&);

vector<vector<double>> ParseHouseholds(const boost::filesystem::path& path);

} // namespace parser
} // namespace geogen
