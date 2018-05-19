//
// Created by robbe on 22/03/18.
//

#include "Utility.h"

#include "util/RunConfigManager.h"

#include "trng/discrete_dist.hpp"
#include "trng/uniform_int_dist.hpp"

#include <cmath>
#include <fstream>
#include <sys/stat.h>

using namespace std;

namespace stride {

bool file_exists(const boost::filesystem::path& path)
{
        struct stat buffer
        {
        };
        return stat(path.string().c_str(), &buffer) == 0;
}

double convert_to_radians(double degrees) { return degrees * M_PI / 180.0; }

vector<unsigned int> generate_random(const vector<double>& p_vec, util::RNManager& rng, unsigned int amount)
{

        trng::discrete_dist  dist(p_vec.begin(), p_vec.end());
        vector<unsigned int> ret_vec;
        for (unsigned int i = 0; i < amount; i++) {
                ret_vec.emplace_back((unsigned int)rng.GetGenerator(dist)());
        }
        return ret_vec;
}

vector<unsigned int> generate_random(unsigned int begin, unsigned int end, util::RNManager& rng, unsigned int amount)
{
        trng::uniform_int_dist dist(begin, end);
        vector<unsigned int>   ret_vec;
        for (unsigned int i = 0; i < amount; i++) {
                ret_vec.emplace_back((unsigned int)rng.GetGenerator(dist)());
        }
        return ret_vec;
}

void check_distribution(vector<double>& p_vec)
{
        bool allzero = true;
        for (const auto& elem : p_vec) {
                if (elem != 0) {
                        allzero = false;
                        break;
                }
        }
        if (allzero)
                for (auto& elem : p_vec)
                        elem += 1;
}

Fractions get_category(const double& age)
{
        if (age >= 26 and age < 65)
                return Fractions::MIDDLE_AGED;
        else if (age >= 3 and age < 18)
                return Fractions::SCHOOLED;
        else if (age >= 18 and age < 26)
                return Fractions::YOUNG;
        else if (age >= 65)
                return Fractions::OLDIES;
        else
                return Fractions::TODDLERS;
}

void ptreeToFile(const boost::property_tree::ptree& pt, const string& fname)
{
        std::ofstream file;
        file.open(fname.c_str(), std::ofstream::out);
        file << util::RunConfigManager::ToString(pt) << endl;
        file.close();
}

} // namespace stride
