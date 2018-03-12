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
class College : public ContactPool {

public:
    College(unsigned int size);

private:
    /// Note from Raphael: this wasn't allowed IIRC,
    /// currently I can't come up with members that make sense
    /// in the future however we should have a vector of persons perhaps?
    /// indicating who goes to this particular college...
    const unsigned int size;
};
} // Namespace geogen

