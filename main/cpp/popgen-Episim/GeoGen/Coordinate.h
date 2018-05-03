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

#include "Utility.h"

#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/strategies/geographic/distance_andoyer.hpp>
#include <boost/geometry/strategies/spherical/distance_haversine.hpp>
#include <boost/geometry/strategies/cartesian/distance_pythagoras.hpp>

namespace bg = boost::geometry;
typedef bg::model::point<double, 2, bg::cs::cartesian>  cPoint;             //carthesian point (x, y)
typedef bg::model::point<double, 2, bg::cs::geographic<bg::degree>> gPoint; //geographical point in degrees (long, lat)

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

        /// Calculates the distance using boost's
        /// @param: c1 The coordinate whose distance has to be calculated from this coordinate.
        /// @param: cartesian A boolean value indicating whether or not we want the cartesian distance
        /// using Pythagoras' theorem.
        /// The default is set to false and thus by default we're looking at the geographical distance
        /// using Andoyer's method.
        /// @retval: <double> Distance between the two coordinates using the given method.
        double GetDistance(const Coordinate& c1, const bool cartesian = false) const;

        /// Getter for the cartesian X coordinate.
        double GetX() const { return bg::get<0>(xy); }

        /// Getter for the cartesian Y coordinate.
        double GetY() const { return bg::get<1>(xy); }

        /// Getter for the geographical longitude coordinate.
        double GetLongitude() const { return bg::get<0>(longlat); }

        /// Getter for the geographical latitude coordinate.
        double GetLatitude() const { return bg::get<1>(longlat); }

private:

        cPoint xy;      ///< The coordinate represented in cartesian format.
        gPoint longlat; ///< The coordinate represented in geographical format.
};

} // namespace stride
