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

    const unsigned int City::getId() const {
        return m_city_id;
    }

    const unsigned int City::getProvince() const {
        return m_province;
    }

    unsigned int City::getPopulation() const {
        return m_population;
    }

    const Coordinate City::getCoordinates() const {
        return m_coordinates;
    }

    const string City::getName() const {
        return m_name;
    }

    void City::addSchool(shared_ptr<School> school) {

        m_schools.push_back(school);
    }

}//namespace geogen