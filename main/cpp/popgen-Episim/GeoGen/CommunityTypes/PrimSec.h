//
// Created by beau on 3/9/18.
//

#ifndef STRIDE_PRIMSEC_H
#define STRIDE_PRIMSEC_H

#include <popgen-Episim/GeoGen/City.h>
#include <popgen-Episim/GeoGen/Household.h>
#include <memory>

namespace geogen {

    enum Type{
        primary,
        secundary
    };

    class PrimSec {
    public:
        PrimSec(){};
        void setType(Type t);
        void setCity(std::shared_ptr<City> c);
        bool addMember(std::shared_ptr<Person> p);

    private:
        /// Primary or secundary community.
        Type type;

        /// The city to which the community belongs.
        std::shared_ptr<City> city;

        /// Members of the community.
        std::vector<std::shared_ptr<Person>> members;
    };
}


#endif //STRIDE_PRIMSEC_H
