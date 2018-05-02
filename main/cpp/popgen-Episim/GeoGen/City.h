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
#include "util/SegmentedVector.h"

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

        /// Get the number of total commuters to the city
        double GetTotalInCommutersCount();

        /// Get the number of total commuters leaving the city
        double GetTotalOutCommutersCount();

        bool HasCollege(){ return m_types_present[CommunityType::College]; }

        /// Returns all the communities
        util::SegmentedVector<Community>& GetAllCommunities(){ return m_communities; };


        /// Returns all the colleges in the city
        vector<Community*> GetColleges() { return m_moc[CommunityType::College]; }

        /// Returns all the schools in the city
        vector<Community*> GetSchools() { return m_moc[CommunityType::School]; }

        /// Returns all the workplaces in the city
        vector<Community*> GetWorkplaces() { return m_moc[CommunityType::Work]; }

        /// Returns all the communities
        //vector<Community*> GetCommunities(); //deprecated?

        /// Returns the primary communities
        vector<Community*> GetPrimaryCommunities() { return m_moc[CommunityType::Primary]; }

        /// Returns the secondary communities
        vector<Community*> GetSecondaryCommunities() { return m_moc[CommunityType::Secondary]; }

        /// Returns the communities of the given type
        /// @param: ct type of the community
        vector<Community*> GetCommunitiesOfType(CommunityType ct) { return m_moc[ct]; }

        /// Adds a new community of the given type to the city.
        /// @param: community_type the type of community that is to be added
        /// @retval: <Community> the recently added community
        Community& AddCommunity(CommunityType community_type);

        /// Set the number of in-coming commuters from the city with the given id
        /// @param: id the id of the city that the commuters come from
        /// @param: number_of_commuters the number of commuters entering to this city
        void SetInCommuters(unsigned int id, double number_of_commuters);

        /// Set the number of out-going commuters from the city with the given id
        /// @param: id the id of the city that the commuters from this city goes to
        /// @param: number_of_commuters the number of commuters leaving this city
        void SetOutCommuters(unsigned int id, double number_of_commuters);

        /// Get all the in-coming commuters
        const map<unsigned int, double>& GetInCommuting() const { return m_in_commuting; };

        /// Get all the outgoing commuters
        const map<unsigned int, double>& GetOutCommuting() const { return m_out_commuting; };

        /// Adds a new household to the city
        Household& AddHousehold(ContactPoolSys& pool_sys);

        /// Gets the households of the city
        vector<Household>& GetHouseholds() { return m_households; }

        /// Returns whether or not the given type of community is present in this city.
        const bool HasCommunityType(CommunityType type) { return m_types_present[type]; }

private:
        const unsigned int m_city_id;   ///< A unique ID of the city.
        const unsigned int m_province;  ///< Province
        unsigned int m_population;      ///< Population of the city.
        Coordinate m_coordinates;       ///< Coordinate, smart coordinate container
        const string m_name;            ///< Name of the city.

        util::SegmentedVector<Community>       m_communities;  ///< Vector of Communities in the city
        map<CommunityType, vector<Community*>> m_moc;          ///< Map of communities for faster retieval

        map<unsigned int, double> m_in_commuting;  ///< Contains number of commuters from other cities to this city
        map<unsigned int, double> m_out_commuting; ///< Contains number of commuters from this city to other cities
        vector<Household> m_households;            ///< contains households of the city

        double m_in_commuter_count;     ///< Number of incomming commuters to the city
        double m_out_commuter_count;    ///< Number of outgoing commuters from the city

        ///< These booleans will improve efficiency...
        bool m_in_commuting_changed;
        bool m_out_commuting_changed;

        map<CommunityType, bool> m_types_present; ///< This map keeps track of the community types present.
};

} // namespace stride
