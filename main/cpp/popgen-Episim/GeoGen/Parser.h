#pragma once
//
// Created by robbe on 6/03/18.
//

#include <memory>
#include <map>

namespace geogen{

std::Map<int, std::shared_ptr<City>> parse_cities(const boost::filesystem::path&);

}