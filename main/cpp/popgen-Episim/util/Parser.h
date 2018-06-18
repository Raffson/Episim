#pragma once
//
// Created by robbe on 6/03/18.
//

#include <map>
#include <vector>

#include "popgen-Episim/model/City.h"
#include "popgen-Episim/util/Utility.h"

#include "util/RNManager.h"

#include <boost/filesystem/path.hpp>
#include <boost/geometry/index/rtree.hpp>

#include "util/CSV.h"
#include "trng/discrete_dist.hpp"
#include <boost/property_tree/xml_parser.hpp>

namespace bgi = boost::geometry::index;
typedef std::pair<gPoint, unsigned int> rtElem;

using namespace std;

namespace stride {
namespace parser {

/// Parse the information of cities.
/// @param path: Path to the file containing information about the city.
/// @param cities: Map with cityIds as key and City as value.
/// @param totalPop: Total population, i.e. the sum of the populations of all cities.
/// @param rtree: A boost rtree used for queries.
void ParseCities(const boost::filesystem::path&, map<unsigned int, City>& cities, unsigned int& totalPop,
                 bgi::rtree<rtElem, bgi::quadratic<16>>& rtree);

/// Parse the commuting information.
/// @param path: Path to the file containing information about commuting.
/// @param cities: Map with cityIds as key and City as value.
/// @param fracs: Distribution of different age-group, working and commuting-behaviour.
void ParseCommuting(const boost::filesystem::path&, map<unsigned int, City>&, const map<Fractions, double>& fracs);

/// Parse the household information.
/// @param path: Path to the file containing information about the commuting behaviour.
/// @retval Composition of the model households.
map<unsigned int, vector<vector<double>>> ParseHouseholds(const boost::filesystem::path& path);

} // namespace parser
} // namespace stride
