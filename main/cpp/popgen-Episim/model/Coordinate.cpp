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

#include "Coordinate.h"


using namespace std;

namespace stride {

Coordinate::Coordinate() : xy(0,0), longlat(0,0) {}

Coordinate::Coordinate(double x, double y, double longitude, double latitude)
    : xy(x,y), longlat(longitude, latitude)
{
}

double Coordinate::GetDistance(const Coordinate& c1, const bool cartesian) const
{
        if( cartesian )
                return (bg::distance(xy, c1.xy) / 1000);
        else
                return (bg::distance(longlat, c1.longlat) / 1000);
}

} // namespace stride
