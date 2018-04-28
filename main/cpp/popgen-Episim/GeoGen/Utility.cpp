//
// Created by robbe on 22/03/18.
//

#include "Utility.h"

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

vector<unsigned int> generate_random(const vector<double>& p_vec, util::RNManager* rng, unsigned int amount)
{

        trng::discrete_dist  dist(p_vec.begin(), p_vec.end());
        vector<unsigned int> ret_vec;
        for (unsigned int i = 0; i < amount; i++) {
                ret_vec.emplace_back((unsigned int)rng->GetGenerator(dist)());
        }
        return ret_vec;
}

vector<unsigned int> generate_random(unsigned int begin, unsigned int end, util::RNManager* rng, unsigned int amount)
{
        trng::uniform_int_dist dist(begin, end);
        vector<unsigned int>   ret_vec;
        for (unsigned int i = 0; i < amount; i++) {
                ret_vec.emplace_back((unsigned int)rng->GetGenerator(dist)());
        }
        return ret_vec;
}

//deprecated?
/*void init_generator(const unsigned long seed, const string& generator_name)
{
        generator.Initialize(util::RNManager::Info(generator_name, seed));
}*/

void check_distribution(vector<double>& p_vec)
{
        bool allzero = true;
        for (auto& elem : p_vec) {
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
        // Same ordering of if-else if as in GeoGrid::GetMainFractions, for the same reasons...
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

} // namespace stride
