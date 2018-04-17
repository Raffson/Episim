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

#include "Community.h"

namespace stride {

using namespace std;

unsigned int Community::m_id_generator = 1;

map<ContactPoolType::Id, unsigned int> Community::m_pool_ids = {{ContactPoolType::Id::Household, 1},
                                                                {ContactPoolType::Id::School, 1},
                                                                {ContactPoolType::Id::Work, 1},
                                                                {ContactPoolType::Id::PrimaryCommunity, 1},
                                                                {ContactPoolType::Id::SecondaryCommunity, 1}};

Community::Community(CommunityType community_type, City* city)
    : m_community_id(m_id_generator++), m_community_type(community_type), m_city(city)
{
}

stride::ContactPool& Community::AddContactPool(ContactPoolType::Id type)
{
        unsigned int id = m_pool_ids.at(type)++;
        m_contact_pools.emplace_back(ContactPool(id, type, this));
        return m_contact_pools.back();
}

unsigned int Community::GetSize() const
{
        unsigned int result = 0;
        for (auto& a_contact_pool : m_contact_pools) {
                result += a_contact_pool.GetSize();
        }

        return result;
}
} // namespace stride
