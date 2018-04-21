//
// Created by robbe on 22/03/18.
//
#pragma once

#include "trng/discrete_dist.hpp"
#include "trng/uniform_int_dist.hpp"
#include "util/RNManager.h"
#include <boost/filesystem/path.hpp>
#include <array>
#include <cmath>
#include <sys/stat.h>
#include <vector>

using namespace std;

namespace stride {

/// Enum that represent indexes of Fractals map. That map contains all fraction data of our population
enum class Fractions
{
        SCHOOLED,           // % of pop that is in school [3yrs, 18yrs),
                            // [3, 6) -> kindergarten, [6, 12) -> elementary school, [12, 18) -> middle+highschool.
        ACTIVE,             // % of pop that is activly working.
        YOUNG,              // % of pop with age [18yrs, 26yrs).
        MIDDLE_AGED,        // % of pop with age [26yrs, 65yrs).
        TODDLERS,           // % of pop that is in  [0yrs, 3yrs).
        OLDIES,             // % of pop that is in [65yrs, oldest_person].
        STUDENTS,           // % of pop in [18, 26) that is enrolled at a college/university.
        COMMUTING_STUDENTS, // % of pop in [18,26) that is enrolled at a college/universit and commutes.
        COMMUTING_WORKERS   // % of pop in [18,65) that works and commutes.
};

/// To allow iteration over the age Fractions.
constexpr std::array<Fractions, 5> AgeList{
    {Fractions::SCHOOLED, Fractions::YOUNG, Fractions::MIDDLE_AGED, Fractions::TODDLERS, Fractions::OLDIES}};

/// To allow iteration over all Fractions.
constexpr std::array<Fractions, 9> FractionList{
        {Fractions::SCHOOLED, Fractions::ACTIVE, Fractions::YOUNG, Fractions::MIDDLE_AGED,
                Fractions::TODDLERS, Fractions::OLDIES, Fractions::STUDENTS,
                Fractions::COMMUTING_STUDENTS, Fractions::COMMUTING_WORKERS}};

    /// Enum that represent indexes of sizes map. That map contains all size data
/// of our communities
enum class Sizes
{
        SCHOOLS,     // Average size of a school.
        COLLEGES,    // Average size of a college.
        COMMUNITIES, // Average size of a community
        WORKPLACES,  // Average size of a workplace
        AVERAGE_CP,  // Average size of a contactpool
        MAXLC        // Amount of largest cities (cities with a college)
};

/// To allow iteration over the Sizes.
constexpr std::array<Sizes, 6> SizeList{
        {Sizes::SCHOOLS, Sizes::COLLEGES, Sizes::COMMUNITIES, Sizes::WORKPLACES, Sizes::AVERAGE_CP, Sizes::MAXLC}};


    /// Checks if a file exists.
/// @param: path A path to a file.
/// @retval: <bool> True if the file exists, False else.
bool file_exists(const boost::filesystem::path& path);

/// Converts a number in degrees to a number in radians.
/// @param degrees a number in degrees
/// @retval <double> a number in radians.
double convert_to_radians(double degrees);

// deprecated?
//void init_generator(const unsigned long seed, const string& generator);

/// Takes a distribution -> a vec of numbers and generates N random numbers given the distribution vector.
/// @param p_vec a vector representing our distribution, each entry contains a relative probabillity.
///              The probabillity to generate the number on that index (first entry is probabillity to
///              generate 0)
/// @param rng  a pointer to a RNMmanager object. See "RNMmanger".h for more info.
/// @param amount default = 1 The amount of random numbers we will generate.
/// @retval <vector<unsigned int>> a vector of unsigned ints of length == amount. Each number is between
///                                [0 and p_vec.size() - 1 ]
/// @example a vec with numbers [1,4,8,2] as relative probabillities, and amount = 3. Will generate
///          3 numbers out of the interval [0,3] with chance on p(0) = 1/15, p(1) = 4/15, p(2) = 8/15, p(3) = 2/15
vector<unsigned int> generate_random(const vector<double>& p_vec, util::RNManager* rng, unsigned int amount = 1);

/// Generates the 'amount' of random unsigned ints between 'begin' and 'end' using 'rng'.
/// @param begin an unsigned int marking the beginning of the random interval
/// @param end an unsigned int marking the end of a random interval, with 'end' not included).
/// @param rng  a pointer to a RNMmanager object. See "RNMmanger".h for more info.
/// @param amount default = 1 The amount of random numbers we will generate.
/// @retval <vector<unsigned int>> a vector of unsigned ints of length == amount. Each number is between
///                                [begin and end - 1 ]
vector<unsigned int> generate_random(unsigned int begin, unsigned int end, util::RNManager* rng, unsigned int amount);

/// Checks the given distribution if all elements are zero. If this is the case,
/// we transform to a uniform distribution. This is needed to take students into account from cities
/// that have no commuters to the largest cities that have a college. Obviously these students need to be
/// commuting to one of these largest cities, therefore we transform to a uniform distribution since
/// a distribution full of zeros will make the generator go bananas. The same can be said for workers, however
/// it is unlikely this case will present itself.
/// @param p_vec A reference to the vector which represents the distribution.
void check_distribution(vector<double>& p_vec);

/// Returns the fraction-category in which 'age' falls.
/// @param age The age for which we want to know the category
/// @retval <Fractions> The corresponsing enum value which represents the category for the given age.
Fractions get_category(const double& age);

namespace constants {

/// Own defined EPSILON, to determine correctness of doubles
// Changed to 10^(-12) after refactor to double...
const double EPSILON = 0.000000000001;
/// Radius of the earth ~= 6371.0 km
const double EARTH_RADIUS = 6371.0; // in kilometer

} // namespace constants
} // namespace stride
