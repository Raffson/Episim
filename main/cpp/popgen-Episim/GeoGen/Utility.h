//
// Created by robbe on 22/03/18.
//
#pragma once

#include <boost/filesystem/path.hpp>
#include <sys/stat.h>
#include <vector>
#include "util/RNManager.h"
#include <cmath>
#include <util/RNManager.h>
#include "trng/discrete_dist.hpp"

using namespace std;

namespace geogen {

/// Checks if a file exists.
/// @param: path A path to a file.
/// @retval: <bool> True if the file exists, False else.
bool file_exists(const boost::filesystem::path& path);

/// Converts a number in degrees to a number in radians.
/// @param degrees a number in degrees
/// @retval <double> a number in radians.
double convert_to_radians(double degrees);

/// Takes a distribution -> a vec of numbers and generates N random numbers given the distribution vector.
/// @param p_vec a vector representing our distribution, each entry contains a relative probabillity.
///              The probabillity to generate the number on that index (first entry is probabillity to
///              generate 0)
/// @param rndm a RNMmanager object. See "RNMmanger".h for more info.
/// @param amount default = 1 The amount of random numbers we will generate.
/// @retval <vector<unsigned int>> a vector of unsigned ints of length == amount. Each number is between
///                                [0 and p_vec.size() - 1 ]
/// @example a vec with numbers [1,4,8,2] as relative probabillities, and amount = 3. Will generate
///          3 numbers out of the interval [0,3] with chance on p(0) = 1/15, p(1) = 4/15, p(2) = 8/15, p(3) = 2/15
vector<unsigned int> generate_random(const vector<double>& p_vec, stride::util::RNManager& rndm, unsigned int amount = 1);

namespace constants {

    /// Own defined EPSILON, to determine correctness of floats
    const double EPSILON   = 0.000001;
    /// Radius of the earth ~= 6371.0 km
    const double EARTH_RADIUS = 6371.0; // in kilometer

}//constants
}//geogen
