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

#include "CommunityType.h"
#include "pool/ContactPoolSys.h"

#include <map>

namespace stride {

class City;

class Community
{
public:
        /// No default constructor
        Community() = delete;

        /// Constructor
        /// @param id The ID to be used for this community
        /// @param type The type of the community to be constructed
        /// @param city A pointer to the city in which this community is located
        Community(const size_t& id, CommunityType::Id type, City* city);

        /// Delete copy constructor and assignment operator
        Community(const Community&)=delete;
        Community& operator=(const Community&)=delete;

        /// Return the community's ID
        size_t GetID() const { return m_id; }

        /// Return the community's type
        const CommunityType::Id& GetCommunityType() const { return m_type; }

        /// Return a const reference of the city,
        City* GetCity() const { return m_city; }

//        /// Return a reference of the city,
//        City* GetCity() { return m_city; }

        /// Adds a new contact pool to the community
        /// @param: poolSys A reference to the ContactPoolSys needed for stride, passed from GeoGrid.
        /// @retval: <ContactPool> The recently added contactpool
        ContactPool& AddContactPool(ContactPoolSys& poolSys);

//        /// Get all the contactpools
//        const std::vector<ContactPool*>& GetContactPools() const { return m_contact_pools; }

        /// Get all the contactpools
        std::vector<ContactPool*>& GetContactPools() { return m_contact_pools; }

private:
        const size_t m_id; ///< A unique ID for the community

        CommunityType::Id m_type; ///< The type of community

        City* m_city; ///< Pointer to City of this community

        std::vector<ContactPool*> m_contact_pools; ///< Contains contactpools
};

} // namespace stride
