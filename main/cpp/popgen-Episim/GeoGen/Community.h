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

#include "pool/ContactPool.h"
#include "pool/ContactPoolSys.h"
#include <cstddef>
#include <map>
#include <memory>

namespace stride {

/// Forward declaration to avoid problems with includes...
class City;

enum class CommunityType
{
        School,
        College,
        Work,
        Primary,
        Secondary
};

/// To allow iteration over the community types.
constexpr std::array<CommunityType, 5> CommunityTypes{{CommunityType::School, CommunityType::College,
                                                       CommunityType::Work, CommunityType::Primary,
                                                       CommunityType::Secondary}};

class Community
{
public:
        ///
        Community() = default;

        /// Constructor, pretty straight forward...
        Community(CommunityType community_type, City* city);

        /// Return the community's ID
        const unsigned int GetID() const { return m_community_id; }

        /// Return the community's type
        const CommunityType& GetCommunityType() const { return m_community_type; }

        /// Return a const reference of the city,
        const City& GetCity() const { return *m_city; }

        /// Return a reference of the city,
        City& GetCity() { return *m_city; }

        /// Adds a new contact pool to the community
        /// @param: pool_sys A reference to the ContactPoolSys needed for stride, passed from GeoGrid.
        /// @retval: <ContactPool> The recently added contactpool
        ContactPool& AddContactPool(ContactPoolSys& pool_sys);

        /// Get all the contactpools
        std::vector<ContactPool*>& GetContactPools() { return m_contact_pools; }

        /// Get the total number members of all contactpools for this community
        unsigned int GetSize() const;

private:
        static unsigned int m_id_generator; ///< Used to generate unique id automatically

        const unsigned int m_community_id; ///< A unique ID for the community

        CommunityType m_community_type; ///< The type of community

        City* m_city; ///< Pointer to City of this community

        static std::map<ContactPoolType::Id, unsigned int> m_pool_ids; ///< Helps to create an unique id for contactpools

        ///< Helps to figure out the ContactPoolType depending on the community's type.
        static std::map<CommunityType, ContactPoolType::Id> m_type_mapper;

        std::vector<ContactPool*> m_contact_pools; ///< Contains contactpools
};

} // namespace stride
