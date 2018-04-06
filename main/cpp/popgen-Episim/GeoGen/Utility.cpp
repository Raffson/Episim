//
// Created by robbe on 22/03/18.
//

#include "Utility.h"

using namespace std;

namespace geogen {

bool file_exists(const boost::filesystem::path& path)
{
        struct stat buffer;
        return stat(path.string().c_str(), &buffer) == 0;
}

} // namespace geogen