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

#include <map>
#include <string>
#include <vector>

#include "Community.h"
#include "Coordinate.h"
#include "Household.h"
#include "util/SegmentedVector.h"

using namespace std;

namespace stride {

class City
{
public:
        /// No default constructor
        City() = delete;

        /// Constructor: set the city data.
        /// @param city_id The ID of this city
        /// @param province The province of this city.
        /// @param population The relative number of people in this city.
        /// @param coordinates The coordinates of this city.
        /// @param name The name of this city.
        City(unsigned int city_id, unsigned int province, unsigned int population, Coordinate coordinates, string name);

        /// Copy constructor
        City(const City&);

        /// Get the id of the city.
        const unsigned int GetId() const { return m_city_id; }

        /// Get the province.
        const unsigned int GetProvince() const { return m_province; }

        /// Get the population of this city
        unsigned int GetPopulation() const { return m_population; }

        /// Get the coordinates of the city
        const Coordinate& GetCoordinates() const { return m_coordinates; }

        /// Get the name of the city
        const string GetName() const { return m_name; };

        /// Get the number of all the communities
        unsigned int GetNrOfCommunities() const { return m_communities.size(); }

        /// Get the number of total commuters to the city
        const double& GetTotalInCommutersCount() const;

        /// Get the number of total commuters leaving the city
        const double& GetTotalOutCommutersCount() const;

        bool HasCollege(){ return m_types_present[CommunityType::Id::College]; }

        /// Returns all the communities
        util::SegmentedVector<Community>& GetAllCommunities() { return m_communities; }

        /// Returns all the communities as a const reference.
        const util::SegmentedVector<Community>& GetAllCommunities() const { return m_communities; }

        /// Returns all the colleges in the city
        const vector<Community*>& GetColleges() { return m_moc[CommunityType::Id::College]; }

        /// Returns all the schools in the city
        const vector<Community*>& GetSchools() { return m_moc[CommunityType::Id::School]; }

        /// Returns all the workplaces in the city
        const vector<Community*>& GetWorkplaces() { return m_moc[CommunityType::Id::Work]; }

        /// Returns all the communities
        //vector<Community*> GetCommunities(); //deprecated?

        /// Returns the primary communities
        const vector<Community*>& GetPrimaryCommunities() { return m_moc[CommunityType::Id::Primary]; }

        /// Returns the secondary communities
        const vector<Community*>& GetSecondaryCommunities() { return m_moc[CommunityType::Id::Secondary]; }

        /// Returns the communities of the given type
        /// @param: ct type of the community
        const vector<Community*>& GetCommunitiesOfType(CommunityType::Id ct) { return m_moc[ct]; }

        /// Adds a new community of the given type to the city.
        /// @param: id The ID of the community to be added
        /// @param: type The type of community to be added
        /// @retval: <Community&> A reference to the recently added community
        Community& AddCommunity(const size_t& id, CommunityType::Id type);

        /// Set the number of incoming commuters from the city with the given id
        /// @param: id the id of the city that the commuters come from
        /// @param: number_of_commuters the number of commuters entering to this city
        void SetInCommuters(unsigned int id, double number_of_commuters);


        /// Set the number of outgoing commuters from the city with the given id
        /// @param: id the id of the city that the commuters from this city goes to
        /// @param: number_of_commuters the number of commuters leaving this city
        void SetOutCommuters(unsigned int id, double number_of_commuters);

        /// Removes an incommuter from the outcommuter list
        /// @param id city id of the city that needs to be removed
        void RemoveInCommuters(unsigned int id);

        /// Removes an outcommuter from the outcommuter list
        /// @param id city of the city that needs to be removed
        void RemoveOutCommuters(unsigned int id);

        /// Get all the incoming commuters
        const map<unsigned int, double>& GetInCommuting() const { return m_in_commuting; };

        /// Get all the outgoing commuters
        const map<unsigned int, double>& GetOutCommuting() const { return m_out_commuting; };

        /// Adds a new household to the city
        /// @param pool_sys The contact pool system.
        Household& AddHousehold(ContactPoolSys& pool_sys);

        /// Gets the households of the city
        util::SegmentedVector<Household>& GetHouseholds() { return m_households; }

        /// Returns whether or not the given type of community is present in this city.
        const bool HasCommunityType(CommunityType::Id type) const { return m_types_present.at(type); }

        unsigned int GetEffectivePopulation() const;
        unsigned int GetInfectedCount() const;

        /// Increments the number of commuters to the destination
        /// @param destination the destination city
        void AddEffectiveCommuterTo(const unsigned int destination);

        /// Returns the number of effective commuters to the destination city from this city
        /// @param destination the destination city of commuters
        unsigned int GetEffectiveCommuterTo(const unsigned int destination);

private:
        const unsigned int m_city_id;   ///< A unique ID of the city.
        const unsigned int m_province;  ///< Province
        unsigned int m_population;      ///< Population of the city according to the model.
        Coordinate m_coordinates;       ///< Coordinate, smart coordinate container
        const string m_name;            ///< Name of the city.

        util::SegmentedVector<Community>           m_communities;  ///< Contains communities of the city
        map<CommunityType::Id, vector<Community*>> m_moc;          ///< Map of communities for faster retrieval
        util::SegmentedVector<Household>           m_households;   ///< Contains households of the city

        map<unsigned int, double> m_in_commuting;  ///< Contains number of commuters from other cities to this city
        map<unsigned int, double> m_out_commuting; ///< Contains number of commuters from this city to other cities

        map<unsigned int, unsigned int> m_effective_out_commuting;

        mutable double m_in_commuter_count;  ///< Number of incomming commuters to the city
        mutable double m_out_commuter_count; ///< Number of outgoing commuters from the city

        ///< These booleans will improve efficiency as they will indicate whether the commuters count has changed.
        ///< If it didn't, we can return the commuters count straight away, else we'll recalculate.
        mutable bool m_in_commuting_changed;
        mutable bool m_out_commuting_changed;

        map<CommunityType::Id, bool> m_types_present; ///< This map keeps track of the community types present.
};

} // namespace stride
