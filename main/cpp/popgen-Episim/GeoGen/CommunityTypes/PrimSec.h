//
// Created by beau on 3/9/18.
//

#ifndef STRIDE_PRIMSEC_H
#define STRIDE_PRIMSEC_H

#include <popgen-Episim/GeoGen/City.h>
#include <popgen-Episim/GeoGen/Household.h>
#include <memory>

namespace geogen {

    using Persons = std::vector<std::shared_ptr<Person>>;

    enum Type{
        primary,
        secundary
    };

    class PrimSec {
    public:
        PrimSec(){};
        void setType(Type t);
        void setCity(std::shared_ptr<City> c);
        void addMember(std::shared_ptr<Person> p);
        std::size_t getNumberMembers(){ return members.size(); }
        std::shared_ptr<City> getCity(){ return city; }
        Type getType(){ return type; }
        Persons getMembers(){ return members; }

    private:
        /// Primary or secundary community.
        Type type;

        /// The city to which the community belongs.
        std::shared_ptr<City> city;

        /// Members of the community.
        Persons members;
    };
}


#endif //STRIDE_PRIMSEC_H
