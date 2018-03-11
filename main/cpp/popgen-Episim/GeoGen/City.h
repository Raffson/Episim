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

#include "popgen-Episim/GeoGen/Coordinate.h"
#include "popgen-Episim/GeoGen/CommunityTypes/School.h"

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

        /// Adds a school to the city.
        /// @param school A school smart_ptr to the school to be added.
        void addSchool(shared_ptr<School> school );


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

        /// vec of schools
        vector<shared_ptr<School>> m_schools;
    };

}//namespace geogen


