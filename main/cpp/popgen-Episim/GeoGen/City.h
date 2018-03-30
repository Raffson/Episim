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
 */

#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "popgen-Episim/GeoGen/Community.h"
#include "popgen-Episim/GeoGen/Coordinate.h"
#include "popgen-Episim/GeoGen/Household.h"

using namespace std;

namespace geogen {

class City
{
public:
        City(unsigned int city_id, unsigned int province, unsigned int population, Coordinate coordinates, string name);

        const unsigned int GetId() const { return m_city_id; };

        const unsigned int GetProvince() const { return m_province; };

        unsigned int GetPopulation() const { return m_population; };

        const Coordinate GetCoordinates() const { return m_coordinates; };

        const string GetName() const { return m_name; };

        unsigned int GetCommunitySize() const { return m_communities.size(); };

        // Raphael@everyone the Communities can still be changed, just not the pointer's value...
        // do we want this behaviour? (see GetHouseholds further down since it's the same story...)
        const vector<shared_ptr<Community>>& GetAllCommunities() { return m_communities; };

        // Raphael@everyone should we keep everything const in the following 6 functions?
        // or should we leave the const off of the vector since it's a copy anyway...
        // or should we leave out all the consts all together so we can mess around with the communitites?
        // my instinct would go for the first option, i.e. leave the const out for the vector itself
        // specifically: vector<shared_ptr<const Community>>
        // mind that the method itself should remain const-qualified in this case...
        const vector<shared_ptr<const Community>> GetColleges() const;

        const vector<shared_ptr<const Community>> GetSchools() const;

        const vector<shared_ptr<const Community>> GetWorkplaces() const;

        const vector<shared_ptr<const Community>> GetCommunities() const;

        const vector<shared_ptr<const Community>> GetPrimaryCommunities() const;

        const vector<shared_ptr<const Community>> GetSecondaryCommunities() const;

        /// Adds community to the city.
        void AddCommunity(shared_ptr<Community> community);

        const map<unsigned int, unsigned int>& GetCommuting() const { return m_commuting; };

        void SetInCommuters(unsigned int id, unsigned int number_of_commuters);

        void AddHousehold(std::shared_ptr<Household> hh);

        // Raphael@Nishchal use const reference here cause even if you don't, the copy
        // will have shared pointers to the households which means the households can still be changed...
        // thus this is simply more efficient, however do we want this behaviour?
        // Raphael@everyone so yeah, do we want the households to be const as well? if so,
        // we'll need to create a copy that will keep the households const...
        const vector<std::shared_ptr<Household>>& GetHouseholds() { return m_households; }

        /// @return number of commuters entering the city
        unsigned int GetNumberOfInCommuters();

private:
        /// A unique ID of the city.
        const unsigned int m_city_id;

        // province
        const unsigned int m_province;

        /// Population of the city.
        unsigned int m_population;

        // Coordinate, smart coordinate container
        Coordinate m_coordinates;

        /// Name of the city.
        const string m_name;

        /// Vector of schools
        vector<shared_ptr<Community>> m_communities;

        /// Contains number of commuters from this city to other cities
        map<unsigned int, unsigned int> m_commuting;

        vector<std::shared_ptr<Household>> m_households;
};

} // namespace geogen
