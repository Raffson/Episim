//
// Created by robbe on 22/03/18.
//

#include "Utility.h"

#include "util/RunConfigManager.h"

#include "trng/discrete_dist.hpp"
#include "trng/uniform_int_dist.hpp"

#include <cmath>
#include <fstream>

#include <boost/filesystem.hpp>

using namespace std;

namespace stride {

bool file_exists(const boost::filesystem::path& path)
{
        return boost::filesystem::exists(path);
}

double convert_to_radians(double degrees) { return degrees * M_PI / 180.0; }

vector<unsigned int> generate_random(const vector<double>& pVec, util::RNManager& rng, unsigned int amount)
{

        trng::discrete_dist  dist(pVec.begin(), pVec.end());
        vector<unsigned int> retVec;
        for (unsigned int i = 0; i < amount; i++) {
                retVec.emplace_back((unsigned int)rng.GetGenerator(dist)());
        }
        return retVec;
}

vector<unsigned int> generate_random(unsigned int begin, unsigned int end, util::RNManager& rng, unsigned int amount)
{
        trng::uniform_int_dist dist(begin, end);
        vector<unsigned int>   retVec;
        for (unsigned int i = 0; i < amount; i++) {
                retVec.emplace_back((unsigned int)rng.GetGenerator(dist)());
        }
        return retVec;
}

void check_distribution(vector<double>& pVec)
{
        bool allzero = true;
        for (const auto& elem : pVec) {
                if (elem != 0) {
                        allzero = false;
                        break;
                }
        }
        if (allzero)
                for (auto& elem : pVec)
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
