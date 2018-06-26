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

Community::Community(const size_t& id, CommunityType::Id type, City* city)
    : m_id(id), m_type(type), m_city(city), m_contact_pools()
{
}

Community::Community(const Community& old)
    : m_id(old.m_id), m_type(old.m_type), m_city(old.m_city), m_contact_pools(old.m_contact_pools)
{
}

stride::ContactPool& Community::AddContactPool(ContactPoolSys& poolSys)
{
        ContactPoolType::Id type = CommunityType::ToContactPoolType(m_type);
#pragma omp critical(contactpoolsys)
    {
        poolSys[type].emplace_back((poolSys[type].size() + 1), type, this);
    }
        m_contact_pools.emplace_back(&poolSys[type].back());
        return (*m_contact_pools.back());
}

unsigned int Community::GetSize() const
{
        unsigned int result = 0;
#pragma omp parallel for reduction(+: result)
        for (auto contactPool = m_contact_pools.begin() ; contactPool < m_contact_pools.end();contactPool++) {
                result += (*contactPool)->GetSize();
        }

        return result;
}

} // namespace stride
