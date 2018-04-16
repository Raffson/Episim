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

namespace stride {

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

        double GetTotalInCommutersCount();

        double GetTotalOutCommutersCount();

        vector<Community>& GetAllCommunities(){ return m_communities; };

        vector<Community*> GetColleges();

        vector<Community*> GetSchools();

        vector<Community*> GetWorkplaces();

        vector<Community*> GetCommunities();

        vector<Community*> GetPrimaryCommunities();

        vector<Community*> GetSecondaryCommunities();

        vector<Community*> GetCommunitiesOfType(CommunityType ct, unsigned int poolsize = 2000,
                                                const bool filter = false);

        /// Adds community to the city.
        Community& AddCommunity(CommunityType community_type);

        void SetInCommuters(unsigned int id, double number_of_commuters);

        void SetOutCommuters(unsigned int id, double number_of_commuters);

        ///
        const map<unsigned int, double>& GetInCommuting() const { return m_in_commuting; };

        const map<unsigned int, double>& GetOutCommuting() const { return m_out_commuting; };

        Household& AddHousehold();

        vector<Household>& GetHouseholds() { return m_households; }

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

        /// Vector of Communities
        vector<Community> m_communities;

        /// Contains number of commuters from this city to other cities
        map<unsigned int, double> m_in_commuting;

        map<unsigned int, double> m_out_commuting;

        vector<Household> m_households;

        double m_in_commuter_count;
        double m_out_commuter_count;

        //These booleans will improve efficiency...
        bool m_in_commuting_changed;
        bool m_out_commuting_changed;

};

} // namespace stride
