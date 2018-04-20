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
 *
 */
namespace stride {

/*
 * Basic Coördinate class. container for x & y value's to represent a
 * position on a grid, map...
 */
class Coordinate
{

public:
        /// Basic constructor
        Coordinate();
        Coordinate(double x, double y, double longitude, double latitude);

        /// Calculates the distance based on haversine formula
        /// credit: https://www.movable-type.co.uk/scripts/latlong.html
        /// @param: c1 the coordinate whose distance has to be calculated from this coordinate
        /// @retval: <double> distance between two coordinates by using longitude and latitude
        double GetDistance(const Coordinate& c1) const;

        double x;       ///< x-coordinate
        double y;       ///< y-coordinate

        ///< a geographic coordinate that specifies the east-west position of a point on the Earth's surface.
        double longitude;

        ///< a geographic coordinate that specifies the north–south position of a point on the Earth's surface.
        double latitude;
};

} // namespace stride
