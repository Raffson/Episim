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
#include <math.h>

using namespace std;

namespace geogen {

Coordinate::Coordinate() : x(0), y(0), longitude(0), latitude(0) {}

Coordinate::Coordinate(double x, double y, double longitude, double latitude)
    : x(x), y(y), longitude(longitude), latitude(latitude)
{
}

double Coordinate::GetDistance(geogen::Coordinate c1)
{
    const double earths_radius = 6371.0; // in kilometer

    double phi1    = c1.latitude * M_PI / 180.0;  // latitude of the first coordinate converted to radian
    double phi2    = latitude * M_PI / 180.0;  // latitude of the second coordinate converted to radian
    double lambda1 = c1.longitude * M_PI / 180.0; // longitude of the first coordinate converted to radian
    double lambda2 = longitude * M_PI / 180.0; // longitude of the second coordinate converted to radian

    double delta_phi    = phi2 - phi1;
    double delta_lambda = lambda2 - lambda1;

    double a = sin(delta_phi / 2.0) * sin(delta_phi / 2.0) +
               cos(phi1) * cos(phi2) * cos(phi1) * cos(phi2) * sin(delta_lambda / 2.0) * sin(delta_lambda / 2.0);

    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));

    return earths_radius * c;
}

} // namespace geogen
