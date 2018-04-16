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
 */

#include "City.h"

using namespace std;

namespace stride {

City::City(const unsigned int city_id, const unsigned int province, unsigned int population,
           const Coordinate coordinates, const string name)
    : m_city_id(city_id), m_province(province), m_population(population), m_coordinates(coordinates), m_name(name),
      m_in_commuter_count(0), m_out_commuter_count(0), m_in_commuting_changed(false), m_out_commuting_changed(false)
{
}

vector<Community*> City::GetColleges()
{
        vector<Community*> colleges;
        for (auto& it : m_communities) {
                if (it.GetCommunityType() == CommunityType::College)
                        colleges.emplace_back(&it);
        }
        return colleges;
}

vector<Community*> City::GetSchools()
{
        vector<Community*> schools;
        for (auto& it : m_communities) {
                if (it.GetCommunityType() == CommunityType::School)
                        schools.emplace_back(&it);
        }
        return schools;
}

vector<Community*> City::GetWorkplaces()
{
        vector<Community*> workplaces;
        for (auto& it : m_communities) {
                if (it.GetCommunityType() == CommunityType::Work)
                        workplaces.emplace_back(&it);
        }
        return workplaces;
}

vector<Community*> City::GetCommunities()
{
        vector<Community*> communities;
        for (auto& it : m_communities) {
                if (it.GetCommunityType() == CommunityType::Primary)
                        communities.emplace_back(&it);
                else if (it.GetCommunityType() == CommunityType::Secondary)
                        communities.emplace_back(&it);
        }
        return communities;
}

vector<Community*> City::GetPrimaryCommunities()
{
        vector<Community*> communities;
        for (auto& it : m_communities) {
                if (it.GetCommunityType() == CommunityType::Primary)
                        communities.emplace_back(&it);
        }
        return communities;
}

vector<Community*> City::GetSecondaryCommunities()
{
        vector<Community*> communities;
        for (auto& it : m_communities) {
                if (it.GetCommunityType() == CommunityType::Secondary)
                        communities.emplace_back(&it);
        }
        return communities;
}

// this is  much better, taking away the need for all the above functions,
// except the functions above make life easier sometimes, so leaving them for now...
vector<Community*> City::GetCommunitiesOfType(CommunityType ct, unsigned int poolsize, const bool filter)
{
        vector<Community*> communities;
        for (auto& it : m_communities) {
                if (it.GetCommunityType() == ct) {
                    if (filter and (ct == CommunityType::Primary or ct == CommunityType::Secondary)) {
                        if (it.GetSize() < poolsize) communities.emplace_back(&it);
                    }
                    else communities.emplace_back(&it);
                }
        }
        return communities;
}

Community& City::AddCommunity(CommunityType community_type) {
    m_communities.emplace_back(Community(community_type, this));
    return m_communities.back();
}

void City::SetInCommuters(unsigned int id, double number_of_commuters)
{
        m_in_commuting_changed = true;
        m_in_commuting[id] = number_of_commuters;
}

void City::SetOutCommuters(unsigned int id, double number_of_commuters)
{
        m_out_commuting_changed = true;
        m_out_commuting[id] = number_of_commuters;
}

double City::GetTotalInCommutersCount()
{
    if( m_in_commuting_changed ) {
        m_in_commuting_changed = false;
        for (auto &it : m_in_commuting)
            m_in_commuter_count += it.second;
    }
    return m_in_commuter_count;
}

double City::GetTotalOutCommutersCount()
{
    if( m_out_commuting_changed ) {
        m_out_commuting_changed = false;
        m_out_commuter_count = 0;
        for (auto &it : m_out_commuting)
            m_out_commuter_count += it.second;
    }
    return m_out_commuter_count;
}


Household& City::AddHousehold() {
    m_households.emplace_back(Household(this));
    return m_households.back();
}

} // namespace stride
