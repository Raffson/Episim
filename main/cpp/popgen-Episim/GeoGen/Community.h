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
 *  Copyright 2017, Draulans S, Van Leeuwen L
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 *
 *  Modified by EpiSim
 */

#include "core/ContactPool.h"
#include <cstddef>
#include <memory>

namespace geogen {

/// Forward declaration to avoid problems with includes...
class City;

enum class CommunityType
{
        School,
        College,
        Work,
        Primary,
        Secondary,
        Null
};

class Community
{
public:
        ///
        Community() = default;

        /// Constructor, pretty straight forward...
        Community(CommunityType community_type, std::shared_ptr<City> city);

        /// Return the community's ID
        const unsigned int GetID() const { return m_community_id; }

        /// Return the community's type
        const CommunityType& GetCommunityType() const { return m_community_type; }

        /// Return shared pointer of the city,
        const std::shared_ptr<City>& GetCity() { return m_city; }

        /// Adds a contact pool to the community
        void AddContactPool(std::shared_ptr<stride::ContactPool> pool);

        //this const qualifier is actually wrong... we need to rework this...
        std::vector<std::shared_ptr<stride::ContactPool>> GetContactPools() const { return m_contact_pools; }

        /// Get the total number members of all contactpools for this community
        int GetSize() const;

private:
        static unsigned int& UIDgenerator();

private:
        const unsigned int m_community_id; ///< A unique ID for the community

        CommunityType m_community_type; ///< The type of community

        std::shared_ptr<City> m_city; ///< Shared pointer to City

        std::vector<std::shared_ptr<stride::ContactPool>> m_contact_pools;

        // Vector of persons to indicate who's part of this community?
        // std::vector<std::shared_ptr<Person>> m_members;
};

} // namespace geogen
