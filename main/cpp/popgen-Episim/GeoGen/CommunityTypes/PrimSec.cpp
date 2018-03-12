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

void PrimSec::addMember(shared_ptr<Person> p) {
    members.push_back(p);
}