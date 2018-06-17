//
// Created by beau on 3/5/18.
//

#include "GeoGrid.h"
#include "popgen-Episim/util/DesignByContract.h"
#include "util/ConfigInfo.h"
#include "util/FileSys.h"
#include "util/TimeStamp.h"

#include "trng/discrete_dist.hpp"
#include "trng/uniform_int_dist.hpp"

#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry/strategies/geographic/distance_andoyer.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <cmath>
#include <iostream>

using namespace std;

namespace stride {

GeoGrid::GeoGrid()
        : m_initial_search_radius(0), m_total_pop(0), m_model_pop(0), m_school_count(0),
          m_population(nullptr), m_rng(), m_random_ages(false)
{
        for( auto frac : FractionList )
            m_fract_map[frac] = 0;

        for( auto size : SizeList )
            m_sizes_map[size] = 0;
}

Coordinate GeoGrid::GetCenterOfGrid()
{
        double smallestX    = numeric_limits<double>::max();
        double biggestX     = numeric_limits<double>::lowest();
        double smallestY    = numeric_limits<double>::max();
        double biggestY     = numeric_limits<double>::lowest();
        double smallestLat  = 180.0;  // Highest lat possible (initial)
        double biggestLat   = -180.0; // Lowest lat  possible (initial)
        double smallestLong = 180.0;  // highest long possible (initial)
        double biggestLong  = -180.0; // lowest long possible (initial)

        // Raphael@everyone, excellent example of something we could run in parallel!!!
        // don't understand me wrong, not right now, but later when we're going for beta or in the final phase...
        for (auto& city : m_cities) {
                Coordinate cc = city.second.GetCoordinates();
                if (cc.GetX() < smallestX)
                        smallestX = cc.GetX();
                if (cc.GetX() > biggestX)
                        biggestX = cc.GetX();
                if (cc.GetY() < smallestY)
                        smallestY = cc.GetY();
                if (cc.GetY() > biggestY)
                        biggestY = cc.GetY();
                if (cc.GetLatitude() < smallestLat)
                        smallestLat = cc.GetLatitude();
                if (cc.GetLatitude() > biggestLat)
                        biggestLat = cc.GetLatitude();
                if (cc.GetLongitude()< smallestLong)
                        smallestLong = cc.GetLongitude();
                if (cc.GetLongitude()> biggestLong)
                        biggestLong = cc.GetLongitude();
        }

        double halfX    = (biggestX - smallestX) / 2;
        double halfY    = (biggestY - smallestY) / 2;
        double halfLat  = (biggestLat - smallestLat) / 2;
        double halfLong = (biggestLong - smallestLong) / 2;

        return {(smallestX + halfX), (smallestY + halfY), (smallestLong + halfLong), (smallestLat + halfLat)};
}

void GeoGrid::DefragmentSmallestCities(double X, double Y, const vector<double>& pVec)
{
        // Step 1: find all cities that have less then Y% of the population
        // popCap: if the population of a city are smaller or equal to this number we defragment them
        auto          popCap = (unsigned int)round(m_total_pop * Y);
        vector<City*> defragCty;
        for (auto& it : m_cities) {
                if (it.second.GetPopulation() <= popCap)
                        defragCty.emplace_back(&it.second);
        }

        // Step2: Decide which X% of cities to delete
        auto toDefrag = (unsigned int)round(defragCty.size() * X);
        while (defragCty.size() > toDefrag) {
                trng::uniform_int_dist distr(0, (unsigned int)defragCty.size() - 1);
                defragCty.erase(defragCty.begin() + m_rng.GetGenerator(distr,(size_t)omp_get_thread_num())());
        }

        // Step 3: replace X% of these cities
        vector<unsigned int> amountToFrag = generate_random(pVec, m_rng, (unsigned int)defragCty.size());
        defragCty.shrink_to_fit();
        unsigned int counter = 0;
        for (auto& it : defragCty) {
                // We add 2 to the amount to defrag, bcs we want to defrag in atleast 2 parts
                for (unsigned int i = 0; i < amountToFrag[counter] + 2; i++) {

                        auto newId    = m_cities.rbegin()->first + 1;
                        auto coords    = it->GetCoordinates();
                        double newLat  = coords.GetLatitude() + pow(-1, i) * (0.1 * i);
                        double newLong = coords.GetLongitude() + pow(-1, i) * (0.1 * i);
                        double newX    = coords.GetX() + pow(-1, i) * (0.1 * i);
                        double newY    = coords.GetY() + pow(-1, i) * (0.1 * i);
                        auto newName = it->GetName();
                        newName += to_string(i);
                        m_cities.emplace(newId,
                                         City(newId, it->GetProvince(), it->GetPopulation() / (amountToFrag[counter] + 2),
                                         Coordinate(newX, newY, newLong, newLat), newName) );
                }
                counter++;
                m_cities.erase(it->GetId());
        }
        // cout << m_cities.size() << endl;
}

const vector<City*>& GeoGrid::GetCitiesWithinRadiusWithCommunityType(const City& origin, unsigned int radius,
                                                                     CommunityType::Id type)
{
        if (!m_neighbours_in_radius[origin.GetId()].count(radius)) {
                unsigned int nextSmaller = m_initial_search_radius;
                while ((radius / nextSmaller) > 1)
                        nextSmaller <<= 1; // equivalent to multiplying by 2.
                unsigned int nextBigger = nextSmaller << 1;
                if ((nextBigger - radius) >= (radius - nextSmaller))
                        radius = nextSmaller;
                else
                        radius = nextBigger;
                //make sure radius does not exceed the limit
#pragma omp critical(radius_geogrid_get_community)
                {
                        radius = min(radius, m_neighbours_in_radius[origin.GetId()].rbegin()->first);
                }
        }
        return m_neighbours_in_radius[origin.GetId()][radius][type];
}

} // namespace stride
