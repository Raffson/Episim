//
// Created by robbe on 22/03/18.
//
#pragma once

#include <boost/filesystem/path.hpp>
#include <sys/stat.h>

namespace geogen {

bool file_exists(const boost::filesystem::path& path);
}
