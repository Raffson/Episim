//
// Created by robbe on 22/03/18.
//
#pragma once

#include <boost/filesystem/path.hpp>
#include <sys/stat.h>
#include <vector>
#include "util/RNManager.h"
#include <cmath>
#include "trng/discrete_dist.hpp"
#include "trng/uniform_int_dist.hpp"
#include <array>

using namespace std;

namespace stride {

/// Enum that represent indexes of Fractals map. That map contains all fraction data of our population
enum class Fractions{
    SCHOOLED,           // % of pop that is in school [3yrs, 18yrs),
                        // [3, 6) -> kindergarten, [6, 12) -> elementary school, [12, 18) -> middle+highschool.
    ACTIVE,             // % of pop that is activly working.
    YOUNG_WORKERS,      // % of pop with age [18yrs, 26yrs) that is working.
    OLD_WORKERS,        // % of pop with age [26yrs, 65yrs).
    TODDLERS,           // % of pop that is in  [0yrs, 3yrs).
    OLDIES,             // % of pop that is in [65yrs, oldest_person].
    STUDENTS,           // % of pop in [18, 26) that is enrolled at a college/university.
    COMMUTING_STUDENTS, // % of pop in [18,26) that is enrolled at a college/universit and commutes.
    COMMUTING_WORKERS   // % of pop in [18,65) that works and commutes.
};

/// To allow iteration over the age Fractions.
    constexpr std::array<Fractions, 5> AgeList{{Fractions::SCHOOLED, Fractions::YOUNG_WORKERS, Fractions::OLD_WORKERS,
                                                       Fractions::TODDLERS, Fractions::OLDIES}};

/// Enum that represent indexes of sizes map. That map contains all size data
/// of our communities
enum class Sizes{
    SCHOOLS,    // Average size of a school.
    COLLEGES,   // Average size of a college.
    COMMUNITIES, // Average size of a community
    WORKPLACES, // Average size of a workplace
    AVERAGE_CP, // Average size of a contactpool
    MAXLC       // Amount of largest cities (cities with a college)
};

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
vector<unsigned int> generate_random(const vector<double>& p_vec, unsigned int amount = 1);

vector<unsigned int> generate_random(unsigned int begin, unsigned  int end, unsigned int amount);

static util::RNManager generator;


namespace constants {

    /// Own defined EPSILON, to determine correctness of doubles
    //Changed to 10^(-12) after refactor to double...
    const double EPSILON   = 0.000000000001;
    /// Radius of the earth ~= 6371.0 km
    const double EARTH_RADIUS = 6371.0; // in kilometer

}//constants
}//stride
