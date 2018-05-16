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

using namespace std;

namespace stride {

unsigned int Community::m_id_generator = 1;

map<ContactPoolType::Id, unsigned int> Community::m_pool_ids = {{ContactPoolType::Id::Household, 1},
                                                                {ContactPoolType::Id::School, 1},
                                                                {ContactPoolType::Id::Work, 1},
                                                                {ContactPoolType::Id::PrimaryCommunity, 1},
                                                                {ContactPoolType::Id::SecondaryCommunity, 1}};

map<CommunityType, ContactPoolType::Id> Community::m_type_mapper = {{CommunityType::School, ContactPoolType::Id::School},
                                                                    {CommunityType::College, ContactPoolType::Id::School},
                                                                    {CommunityType::Work, ContactPoolType::Id::Work},
                                                                    {CommunityType::Primary, ContactPoolType::Id::PrimaryCommunity},
                                                                    {CommunityType::Secondary, ContactPoolType::Id::SecondaryCommunity}};

Community::Community(CommunityType communityType, City* city)
    : m_community_id(m_id_generator++), m_community_type(communityType), m_city(city)
{
}

stride::ContactPool& Community::AddContactPool(ContactPoolSys& poolSys)
{
        ContactPoolType::Id type = m_type_mapper[m_community_type];
        unsigned int id = m_pool_ids.at(type)++;
        poolSys[type].emplace_back(ContactPool(id, type, this));
        m_contact_pools.emplace_back(&poolSys[type].back());
        return (*m_contact_pools.back());
}

unsigned int Community::GetSize() const
{
        unsigned int result = 0;
        for (auto& contactPool : m_contact_pools) {
                result += contactPool->GetSize();
        }

        return result;
}
} // namespace stride
