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
 */

/**
 * @file
 * Definition of ClusterType.
 */

#include <cstddef>
#include <string>

namespace stride {
namespace ClusterType {

/// Enumerates the cluster types.
enum class Id
{
        Household,
        School,
        Work,
        PrimaryCommunity,
        SecondaryCommunity,
        Null
};

/// Number of Cluster types (not including Null type).
inline constexpr unsigned int NumOfTypes() { return 5U; }

/// Cast to size_t for indexing.
inline std::size_t ToSizeT(Id id) { return static_cast<std::size_t>(id); }

/// Converts a ClusterType::Id value to corresponding name.
std::string ToString(ClusterType::Id w);

/// Check whether string is name of a ClusterType::Id.
bool IsType(const std::string& s);

/// Converts a string with name to ClusterType::Id.
Id ToType(const std::string& s);

} // namespace ClusterType
} // namespace stride