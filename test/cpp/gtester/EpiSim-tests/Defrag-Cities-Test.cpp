//
// Created by robbe on 12/04/18.
//

#include "popgen-Episim/generator/GeoGridGenerator.h"
#include <vector>

#include <gtest/gtest.h>

namespace Tests {

using namespace std;
using namespace ::testing;
using namespace stride;

using boost::property_tree::ptree;

class DefragCityTest : public ::testing::TestWithParam<unsigned int>
{
public:
        DefragCityTest() : geo() {}

protected:
        shared_ptr<GeoGrid> geo;
        /// Destructor has to be virtual.
        ~DefragCityTest() override = default;

        /// Set up for the test fixture
        void SetUp() override
        {
                geo = GeoGridGenerator().Generate("run_default.xml");
        }

        /// Tearing down the test fixture
        void TearDown() override { }
};

TEST_P(DefragCityTest, happy_defrag)
{
        auto ctyAmount = (unsigned int) geo->GetCities().size();
        double         X     = 0.5;  // We will fragment half of the chosen cities
        double         Y     = 1.0; // should be all cities
        vector<double> pVec = {1};
        geo->DefragmentSmallestCities(X, Y, pVec);

        EXPECT_NEAR(ctyAmount  + ctyAmount / 2 , geo->GetCities().size(), 1);
}

TEST_P(DefragCityTest, value_test)
{

        City* smallestCity = &geo->GetCities().begin()->second;

        for (auto& it : geo->GetCities()) {

                if (smallestCity->GetPopulation() > it.second.GetPopulation()) {
                        smallestCity = &it.second;
                }
        }

        double         Y     = ((double)smallestCity->GetPopulation() / (double)geo->GetTotalPop());
        double         X     = 1.0;
        vector<double> pVec = {1};

        Coordinate   oldCoordinate = smallestCity->GetCoordinates();
        string       oldName       = smallestCity->GetName();
        unsigned int oldProvince   = smallestCity->GetProvince();
        unsigned int oldLastId    = geo->GetCities().rbegin()->second.GetId();
        unsigned int ctyCount = geo->GetCities().size();
        geo->DefragmentSmallestCities(X, Y, pVec);

        ASSERT_EQ(ctyCount + 1, geo->GetCities().size()); // Test if we only fragmented one city

        City* lastCity = &geo->GetCities().rbegin()->second;
        auto  rIt      = geo->GetCities().rbegin();
        rIt++;
        City* secondLastCity = &rIt->second;
        ASSERT_NE(lastCity, secondLastCity);

        // Splitted in 2 cities. Coordinates of the first city should match the original city
        EXPECT_FLOAT_EQ(oldCoordinate.GetLongitude(), secondLastCity->GetCoordinates().GetLongitude());
        EXPECT_FLOAT_EQ(oldCoordinate.GetLatitude(), secondLastCity->GetCoordinates().GetLatitude());
        EXPECT_FLOAT_EQ(oldCoordinate.GetX(), secondLastCity->GetCoordinates().GetX());
        EXPECT_FLOAT_EQ(oldCoordinate.GetY(), secondLastCity->GetCoordinates().GetY());

        // Coordinates of the second city are 0.1 off the original
        EXPECT_FLOAT_EQ(oldCoordinate.GetLongitude() - 0.005, lastCity->GetCoordinates().GetLongitude());
        EXPECT_FLOAT_EQ(oldCoordinate.GetLatitude() - 0.005, lastCity->GetCoordinates().GetLatitude());
        EXPECT_FLOAT_EQ(oldCoordinate.GetX() - 0.1, lastCity->GetCoordinates().GetX());
        EXPECT_FLOAT_EQ(oldCoordinate.GetY() - 0.1, lastCity->GetCoordinates().GetY());

        // Province check, should be the same
        EXPECT_EQ(oldProvince, secondLastCity->GetProvince());
        EXPECT_EQ(oldProvince, lastCity->GetProvince());

        // New ID check
        EXPECT_EQ(oldLastId + 1, secondLastCity->GetId());
        EXPECT_EQ(oldLastId + 2, lastCity->GetId());

        // New names
        EXPECT_EQ(oldName + " 0", secondLastCity->GetName());
        EXPECT_EQ(oldName + " 1", lastCity->GetName());
}

namespace {
// OpenMP should have no effect atm...
#ifdef _OPENMP
unsigned int threads[]{1U};
#else
unsigned int threads[]{1U};
#endif

} // namespace

INSTANTIATE_TEST_CASE_P(Run, DefragCityTest, ValuesIn(threads));
} // namespace Tests
