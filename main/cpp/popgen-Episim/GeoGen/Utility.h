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

bool file_exists(const boost::filesystem::path& path);
double convert_to_radians(double degrees);
vector<unsigned int> generate_random(const vector<double>& p_vec, stride::util::RNManager& rndm, unsigned int amount = 1);

namespace constants {

    const double EPSILON   = 0.000001;
    const double EARTH_RADIUS = 6371.0; // in kilometer

}//constants
}//geogen
