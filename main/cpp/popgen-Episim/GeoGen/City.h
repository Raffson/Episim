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
        /// Constructor: set the city data.
        City(unsigned int city_id, unsigned int province, unsigned int population, Coordinate coordinates, string name);

        /// Get the id of the city.
        const unsigned int GetId() const { return m_city_id; };

        /// Get the province.
        const unsigned int GetProvince() const { return m_province; };

        /// Get the population of this city
        unsigned int GetPopulation() const { return m_population; };

        /// Get the coordinates of the city
        const Coordinate GetCoordinates() const { return m_coordinates; };

        /// Get the name of the city
        const string GetName() const { return m_name; };

        /// Get the size of all the communities
        unsigned int GetCommunitySize() const { return m_communities.size(); };

        double GetTotalInCommutersCount();

        double GetTotalOutCommutersCount();

        /// Returns all the communities
        vector<Community>& GetAllCommunities(){ return m_communities; };

        /// Returns all the colleges in the city
        vector<Community*> GetColleges();

        /// Returns all the schools in the city
        vector<Community*> GetSchools();

        /// Returns all the workplaces in the city
        vector<Community*> GetWorkplaces();

        vector<Community*> GetCommunities();

        vector<Community*> GetPrimaryCommunities();

        vector<Community*> GetSecondaryCommunities();

        vector<Community*> GetCommunitiesOfType(CommunityType ct, unsigned int poolsize = 2000,
                                                const bool filter = false);

        /// Adds community to the city.
        Community& AddCommunity(CommunityType community_type);

        /// Set the number of in-coming commuters from the city with the given id
        void SetInCommuters(unsigned int id, double number_of_commuters);

        /// Set the number of out-going commuters from the city with the given id
        void SetOutCommuters(unsigned int id, double number_of_commuters);

        ///
        const map<unsigned int, double>& GetInCommuting() const { return m_in_commuting; };

        const map<unsigned int, double>& GetOutCommuting() const { return m_out_commuting; };

        Household& AddHousehold();

        vector<Household>& GetHouseholds() { return m_households; }

private:
        const unsigned int m_city_id;   ///< A unique ID of the city.
        const unsigned int m_province;  ///< Province
        unsigned int m_population;      ///< Population of the city.
        Coordinate m_coordinates;       ///< Coordinate, smart coordinate container
        const string m_name;            ///< Name of the city.

        vector<Community> m_communities;           ///< Vector of Communities in the city
        map<unsigned int, double> m_in_commuting;  ///< Contains number of commuters from other cities to this city
        map<unsigned int, double> m_out_commuting; ///< Contains number of commuters from this city to other cities
        vector<Household> m_households;             ///< contains households of the city

        double m_in_commuter_count;     ///< Number of incomming commuters to the city
        double m_out_commuter_count;    ///< Number of outgoing commuters from the city


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

        // These booleans will improve efficiency...
        bool m_in_commuting_changed;
        bool m_out_commuting_changed;
};

} // namespace stride
