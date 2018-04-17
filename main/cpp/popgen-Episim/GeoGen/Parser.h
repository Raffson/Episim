#pragma once
//
// Created by robbe on 6/03/18.
//

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "util/CSV.h"
#include "util/CSVRow.h"
#include "util/RNManager.h"

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "popgen-Episim/GeoGen/City.h"
#include "popgen-Episim/GeoGen/Utility.h"

#include "trng/discrete_dist.hpp"

using namespace std;

namespace stride {
namespace parser {

/// Parse the information of cities
void ParseCities(const boost::filesystem::path&, map<unsigned int, City>& cities, unsigned int& total_pop);

/// Parse the commuting information
void ParseCommuting(const boost::filesystem::path&, map<unsigned int, City>&, const map<Fractions, double>& fracs);

vector<City> DefragmentCity(const City& ct, vector<double> distr, util::RNManager& rndm);

/// Parse the household information
vector<vector<double>> ParseHouseholds(const boost::filesystem::path& path);

} // namespace parser
} // namespace stride
