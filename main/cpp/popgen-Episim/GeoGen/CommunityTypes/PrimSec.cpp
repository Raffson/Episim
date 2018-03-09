//
// Created by beau on 3/9/18.
//

#include "PrimSec.h"

using namespace geogen;

void PrimSec::setType(Type t) {
    type = t;
}

void PrimSec::setCity(shared_ptr<City> c) {
    city = c;
}

bool PrimSec::addMember(shared_ptr<Person> p) {
    if (members.size() < 2000){
        members.push_back(p);
        return true;
    }
    return false;
}