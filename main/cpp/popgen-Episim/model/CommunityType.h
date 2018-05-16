#pragma once
/*
 *  This is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  The software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with the software. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2017, Kuylen E, Willem L, Broeckhove J
 *  Copyright 2018, Raphael A
 */

/**
 * @file
 * Definition of CommunityType.
 */

#include "pool/ContactPoolType.h"
#include "popgen-Episim/util/Utility.h"

namespace stride {
namespace CommunityType {

/// Enumerates the Community types.
enum class Id : std::size_t
{
        School,
        College,
        Work,
        Primary,
        Secondary,
};

/// Number of Community types.
inline constexpr unsigned int NumOfTypes() { return 5U; }

/// Check whether string is name of a CommunityType::Id.
bool IsType(const std::string& s);

/// Cast to size_t for indexing.
inline constexpr std::size_t ToSizeT(Id id) { return static_cast<std::size_t>(id); }

/// Converts a ContactPoolType::Id value to corresponding name.
std::string ToString(Id c);

/// Converts a string with name to Id.
Id ToType(const std::string& s);

/// Returns the average size for this community type.
Sizes ToSizes(Id id);

/// Converts to the corresponding ContactPool type
ContactPoolType::Id ToContactPoolType(Id id);

/// To allow iteration over the community types.
constexpr std::array<Id, 5> IdList{{Id::School, Id::College, Id::Work, Id::Primary, Id::Secondary}};

} // namespace ContactPoolType
} // namespace stride
