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


#include <string>
#include <utility>
#include <cstddef>
#include <vector>
#include <memory>
#include <map>

#include "popgen-Episim/GeoGen/Coordinate.h"
#include "popgen-Episim/GeoGen/Community.h"

using namespace std;

namespace geogen {

    class City {
    public:

        City(unsigned int city_id, unsigned int province, unsigned int population,
             Coordinate coordinates, string name);

        const unsigned int getId() const;

        const unsigned int getProvince() const;

        unsigned int getPopulation() const;

        const Coordinate getCoordinates() const;

        const string getName() const;

        unsigned int getCommunitySize() const;

        const vector<shared_ptr<Community>>& getAllCommunities();

        const vector<shared_ptr<const Community>> getColleges() const;

        unsigned int getNumberOfWorkplaces() const;

        /// Adds community to the city.
        void addCommunity(shared_ptr<Community> community);

        map<unsigned int, unsigned int> GetCommuting() const{return m_commuting;};

        void SetInCommuters(unsigned int id, unsigned int number_of_commuters);

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
        map<unsigned int, unsigned int>   m_commuting;
    };

}//namespace geogen


