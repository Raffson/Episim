//
// Created by robbe on 22/03/18.
//

#include <math.h>
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

} // namespace geogen
