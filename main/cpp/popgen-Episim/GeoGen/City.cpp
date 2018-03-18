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
                        const Coordinate coordinates, const string name) : m_city_id(city_id),
                                                                           m_province(province),
                                                                           m_population(population),
                                                                           m_coordinates(coordinates),
                                                                           m_name(name) {}

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

    const vector<shared_ptr<const Community>> City::GetColleges() const {
        vector<shared_ptr<const Community>> colleges;
        for( auto &it : m_communities ) {
            if( it->GetCommunityType() == CommunityType::College ) colleges.push_back(it);
        }
        return colleges;
    }

    unsigned int City::GetNumberOfWorkplaces() const{
        unsigned int result = 0;
        for(auto a_community:m_communities){
            if(a_community->GetCommunityType() == CommunityType::Work){
                result++;
            }
        }
        return result;
    }

    void City::AddCommunity(shared_ptr<Community> community) {
        m_communities.push_back(community);
    }


    void City::SetInCommuters(unsigned int id, unsigned int number_of_commuters){
        m_commuting[id] = number_of_commuters;
    }

    unsigned int City::GetNumberOfInCommuters(){
        unsigned int result = 0;
        for(auto it:m_commuting){
            result += it.second;
        }
        return result;

    }


}//namespace geogen