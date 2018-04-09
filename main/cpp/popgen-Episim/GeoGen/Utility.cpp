//
// Created by robbe on 22/03/18.
//


#include "Utility.h"


using namespace std;

namespace geogen {

bool file_exists(const boost::filesystem::path& path)
{
        struct stat buffer{};
        return stat(path.string().c_str(), &buffer) == 0;
}

double convert_to_radians(double degrees) {

    return degrees * M_PI / 180.0;
}

vector<unsigned int>
generate_random(const vector<double> &p_vec, stride::util::RNManager &rndm, unsigned int amount) {

    trng::discrete_dist dist(p_vec.begin(), p_vec.end());
    vector<unsigned int> ret_vec;
    for(unsigned int i = 0; i < amount; i++){
        ret_vec.emplace_back(rndm.GetGenerator(dist)());
    }
    return ret_vec;
}

} // namespace geogen
