#pragma once
//
// Created by robbe on 10/03/18.
//

#include "popgen-Episim/GeoGen/CommunityTypes/ContactPool.h"

namespace geogen {

/**
 * School class. Will be used to represent school object
 * Base class ContactPool.
 */
class School : public ContactPool {

public:
    School(unsigned int size);

private:
    const unsigned int size;
};
} // Namespace geogen

