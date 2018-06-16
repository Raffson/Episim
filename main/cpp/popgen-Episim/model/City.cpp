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
        for( auto type : CommunityType::IdList )
                m_types_present[type] = false;

}

City::City(const City& cty): m_city_id(cty.m_city_id), m_province(cty.m_province), m_population(cty.m_population),
                             m_coordinates(cty.m_coordinates), m_name(cty.m_name),m_in_commuter_count(cty.m_in_commuter_count),
                             m_out_commuter_count(cty.m_in_commuter_count), m_in_commuting_changed(true),
                             m_out_commuting_changed(true), m_in_commuting(cty.m_in_commuting), m_out_commuting(cty.m_out_commuting)


{
    m_in_commuting = cty.m_in_commuting;
    m_out_commuting = cty.m_out_commuting;
    m_types_present = cty.m_types_present;

}


Community& City::AddCommunity(const size_t& id, CommunityType::Id type)
{
        m_communities.emplace_back(id, type, this);
        m_types_present[type] = true;
        m_moc[type].emplace_back(&m_communities.back());
        return m_communities.back();
}

void City::SetInCommuters(unsigned int id, double number_of_commuters)
{
        m_in_commuting_changed = true;
        m_in_commuting[id]     = number_of_commuters;
}

void City::SetOutCommuters(unsigned int id, double number_of_commuters)
{
        m_out_commuting_changed = true;
        m_out_commuting[id]     = number_of_commuters;
}

const double& City::GetTotalInCommutersCount() const
{
        if (m_in_commuting_changed) {
                m_in_commuting_changed = false;
                m_in_commuter_count    = 0;
                for (const auto& it : m_in_commuting)
                        m_in_commuter_count += it.second;
                m_in_commuter_count -= m_in_commuting.at(m_city_id);
        }
        return m_in_commuter_count;
}

const double& City::GetTotalOutCommutersCount() const
{
        if (m_out_commuting_changed) {
                m_out_commuting_changed = false;
                m_out_commuter_count    = 0;
                for (const auto& it : m_out_commuting)
                        m_out_commuter_count += it.second;
                m_out_commuter_count -= m_out_commuting.at(m_city_id);
        }
        return m_out_commuter_count;
}

Household& City::AddHousehold(ContactPoolSys& pool_sys)
{
        m_households.emplace_back(this, pool_sys);
        return m_households.back();
}

unsigned int City::GetEffectivePopulation() const
{
        unsigned int result = 0;
        for(auto& hh:m_households){
                result += hh.GetSize();
        }
        return result;
}


unsigned int City::GetInfectedCount() const
{
    unsigned int result = 0;
    for(auto& hh:m_households){
        for(auto& aPerson:hh.GetMembers()){
            if(aPerson->GetHealth().IsInfected() || aPerson->GetHealth().IsRecovered()){
                result++;
            }
        }
    }
    return result;
}

void City::AddEffectiveCommuterTo(const unsigned int destination)
{
#pragma omp critical(m_effective)
    {
        if (m_effective_out_commuting.find(destination) == m_effective_out_commuting.end()) {

            m_effective_out_commuting[destination] = 1;
        } else {
            m_effective_out_commuting[destination] = m_effective_out_commuting[destination] + 1;
        }
    }
}


unsigned int City::GetEffectiveCommuterTo(const unsigned int destination)
{
    if(m_effective_out_commuting.find(destination) == m_effective_out_commuting.end()){
        return 0;
    }
    else{
        return m_effective_out_commuting[destination];
    }
}

    void City::RemoveInCommuters(unsigned int id) {

        m_in_commuting.erase(id);
        m_in_commuting_changed = true;

    }

    void City::RemoveOutCommuters(unsigned int id) {
        m_out_commuting.erase(id);
        m_out_commuting_changed = true;
    }

} // namespace stride
