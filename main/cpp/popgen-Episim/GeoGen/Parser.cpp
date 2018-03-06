//
// Created by robbe on 6/03/18.
//

#include "Parser.h"

namespace geogen {
namespace parser{


    std::map<int, std::shared_ptr<City>> parse_cities(const boost::filesystem::path& filename){
        std::map<int, std::shared_ptr<City>> result;
        return result;
    };

    std::shared_ptr<City> parse_city(const stride::util::CSVRow& row){

    };

} // namespace parser
} // namespace geogen

