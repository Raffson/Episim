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
 * Header for the core ContactPool class.
 */

#include "contact/ContactLogMode.h"
#include "pool/ContactPoolType.h"

#include <array>
#include <vector>

namespace stride {

class Person;
class Community;
class Household;

/**
 * Represents a group of Persons that potentially have contacts.
 */
class ContactPool
{
public:
        /// Initializing constructor.
        ContactPool(std::size_t pool_id, ContactPoolType::Id type);

        /// Initializing constructor for a contact pool that belongs to a community.
        ContactPool(std::size_t pool_id, ContactPoolType::Id type, Community* community);

        /// Initializing constructor for a contact pool that belongs to a household.
        ContactPool(std::size_t pool_id, ContactPoolType::Id type, Household* house);

            /// Get the ID of the pool
        std::size_t GetID() const { return m_pool_id; }

        /// Get the community to which this pool belongs
        Community* GetCommunity() { return m_community; }

        /// Get the household to which this pool belongs
        Household* GetHousehold() { return m_household; }

            /// Add the given Person.
        void AddMember(const Person* p);

        /// Get member at index.
        Person* GetMember(unsigned int index) const { return m_members[index]; }

        /// Get the entire pool of members.
        const std::vector<Person*>& GetPool() const { return m_members; }

        /// Get size (number of members).
        std::size_t GetSize() const { return m_members.size(); }

private:
        /// Sort members w.r.t. health status.
        /// Sort order: exposed/infected/recovered, susceptible, immune).
        std::tuple<bool, size_t> SortMembers();

        /// Infector calculates contacts and transmissions.
        template <ContactLogMode::Id LL, bool TIC, typename LIP, bool TO>
        friend class Infector;

private:
        std::size_t          m_pool_id;      ///< The ID of the ContactPool (for logging purposes).
        ContactPoolType::Id  m_pool_type;    ///< The type of the ContactPool (for logging purposes).
        std::size_t          m_index_immune; ///< Index of the first immune member in the ContactPool.
        std::vector<Person*> m_members;      ///< Container with pointers to contactpool members.
        Community*           m_community;    ///< Pointer to the community that holds this pool.
        Household*           m_household;    ///< Pointer to the household that holds this pool.
};

} // namespace stride
