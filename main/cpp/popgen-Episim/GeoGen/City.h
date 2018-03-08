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

#include "popgen-Episim/GeoGen/Coordinate.h"

using namespace std;
namespace geogrid {
    class City {
    public:

        City(unsigned int city_id, unsigned int province, unsigned int population,
             Coordinate coordinates, string name);

        unsigned int getPopulation() const;

        const string getName() const;

        const unsigned int getId() const;

    private:

        /// A unique ID of the city.
        const unsigned int city_id;

        // province
        const unsigned int province;

        /// Population of the city.
        unsigned int population;

        // Coordinate, smart coordinate container
        Coordinate coordinates;

        /// Name of the city.
        const string name;
    };
}//namespace geogen


