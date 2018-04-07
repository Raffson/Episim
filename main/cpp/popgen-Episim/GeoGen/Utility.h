//
// Created by robbe on 22/03/18.
//
#pragma once

#include <boost/filesystem/path.hpp>
#include <sys/stat.h>

namespace geogen {

bool file_exists(const boost::filesystem::path& path);
double convert_to_radians(double degrees);

namespace constants {

    const double EPSILON   = 0.000001;
    const double EARTH_RADIUS = 6371.0; // in kilometer

}//constants
}//geogen
