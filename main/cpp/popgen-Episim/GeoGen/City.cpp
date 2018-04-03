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

namespace geogen {

City::City(const unsigned int city_id, const unsigned int province, unsigned int population,
           const Coordinate coordinates, const string name)
    : m_city_id(city_id), m_province(province), m_population(population), m_coordinates(coordinates), m_name(name),
      m_in_commuter_count(0), m_out_commuter_count(0), m_in_commuting_changed(false), m_out_commuting_changed(false)
{
}
/*
    const unsigned int City::GetId() const {
        return m_city_id;
    }

    const unsigned int City::GetProvince() const {
        return m_province;
    }

    unsigned int City::GetPopulation() const {
        return m_population;
    }

    const Coordinate City::GetCoordinates() const {
        return m_coordinates;
    }

    const string City::GetName() const {
        return m_name;
    }

    unsigned int City::GetCommunitySize() const {
        return m_communities.size();
    }

    const vector<shared_ptr<Community>>& City::GetAllCommunities() {
        return m_communities;
    }
*/

vector<shared_ptr<Community>> City::GetColleges()
{
        vector<shared_ptr<Community>> colleges;
        for (auto& it : m_communities) {
                if (it->GetCommunityType() == CommunityType::College)
                        colleges.push_back(it);
        }
        return colleges;
}

vector<shared_ptr<Community>> City::GetSchools()
{
        vector<shared_ptr<Community>> schools;
        for (auto& it : m_communities) {
                if (it->GetCommunityType() == CommunityType::School)
                        schools.push_back(it);
        }
        return schools;
}

vector<shared_ptr<Community>> City::GetWorkplaces()
{
        vector<shared_ptr<Community>> workplaces;
        for (auto& it : m_communities) {
                if (it->GetCommunityType() == CommunityType::Work)
                        workplaces.push_back(it);
        }
        return workplaces;
}

vector<shared_ptr<Community>> City::GetCommunities()
{
        vector<shared_ptr<Community>> communities;
        for (auto& it : m_communities) {
                if (it->GetCommunityType() == CommunityType::Primary)
                        communities.push_back(it);
                else if (it->GetCommunityType() == CommunityType::Secondary)
                        communities.push_back(it);
        }
        return communities;
}

vector<shared_ptr<Community>> City::GetPrimaryCommunities()
{
        vector<shared_ptr<Community>> communities;
        for (auto& it : m_communities) {
                if (it->GetCommunityType() == CommunityType::Primary)
                        communities.push_back(it);
        }
        return communities;
}

vector<shared_ptr<Community>> City::GetSecondaryCommunities()
{
        vector<shared_ptr<Community>> communities;
        for (auto& it : m_communities) {
                if (it->GetCommunityType() == CommunityType::Secondary)
                        communities.push_back(it);
        }
        return communities;
}

// this is  much better, taking away the need for all the above functions,
// except the functions above make life easier sometimes, so leaving them for now...
vector<shared_ptr<Community>> City::GetCommunitiesOfType(CommunityType ct)
{
        vector<shared_ptr<Community>> communities;
        for (auto& it : m_communities) {
                if (it->GetCommunityType() == ct)
                        communities.push_back(it);
        }
        return communities;
}

void City::AddCommunity(shared_ptr<Community> community) { m_communities.push_back(community); }

void City::SetInCommuters(unsigned int id, unsigned int number_of_commuters)
{
        m_in_commuting_changed = true;
        m_in_commuting[id] = number_of_commuters;
}

void City::SetOutCommuters(unsigned int id, unsigned int number_of_commuters)
{
        m_out_commuting_changed = true;
        m_out_commuting[id] = number_of_commuters;
}

unsigned int City::GetTotalInCommutersCount()
{
    if( m_in_commuting_changed ) {
        m_in_commuting_changed = false;
        m_in_commuter_count = 0;
        for (auto &it : m_in_commuting)
            m_in_commuter_count += it.second;
    }
    return m_in_commuter_count;
}

unsigned int City::GetTotalOutCommutersCount()
{
    if( m_out_commuting_changed ) {
        m_out_commuting_changed = false;
        m_out_commuter_count = 0;
        for (auto &it : m_out_commuting)
            m_out_commuter_count += it.second;
    }
    return m_out_commuter_count;
}


void City::AddHousehold(std::shared_ptr<Household> hh) { m_households.push_back(hh); }


} // namespace geogen
