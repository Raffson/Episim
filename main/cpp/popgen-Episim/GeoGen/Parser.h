#pragma once
//
// Created by robbe on 6/03/18.
//

#include <algorithm>
#include <map>
#include <vector>

#include "popgen-Episim/GeoGen/City.h"
#include "popgen-Episim/GeoGen/Utility.h"

#include "util/RNManager.h"

#include <boost/filesystem.hpp>


using namespace std;

namespace stride {
namespace parser {

/// Parse the information of cities
/// @param: path to the file containing information about the city
/// @param: cities map city_ids as key and City as the value
/// @param: total_pop: total population got from the sum of all the cities
void ParseCities(const boost::filesystem::path&, map<unsigned int, City>& cities, unsigned int& total_pop);

/// Parse the commuting information
/// @param: path to the file containing information about commuting
/// @param: cities map city_ids as key and City as the value
/// @param: fracs distribution of different age-group, working and commuting-behaviour
void ParseCommuting(const boost::filesystem::path&, map<unsigned int, City>&, const map<Fractions, double>& fracs);

vector<City> DefragmentCity(const City& ct, vector<double> distr, util::RNManager& rndm);

/// Parse the household information
/// @param: path to the file containing information about the commuting behaviour
/// @retval vector<vector<double>> composition of the model households
map<unsigned int, vector<vector<double>>> ParseHouseholds(const boost::filesystem::path& path);

} // namespace parser
} // namespace stride
