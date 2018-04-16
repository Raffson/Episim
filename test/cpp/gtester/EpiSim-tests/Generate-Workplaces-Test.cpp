//
// Created by beau on 3/15/18.
//

/**
 * @file
 * Implementation of tests to check the generation of communities.
 */

#include "popgen-Episim/GeoGen/GeoGrid.h"

#include <boost/property_tree/ptree.hpp>
#include <gtest/gtest.h>
#include <omp.h>
#include <spdlog/spdlog.h>

namespace Tests {

using namespace std;
using namespace ::testing;
using namespace stride;

using boost::property_tree::ptree;

class WorkplaceTest : public ::testing::TestWithParam<unsigned int>
{
public:
        /// TestCase set up.
        static void SetUpTestCase() {}

        /// Tearing down TestCase
        static void TearDownTestCase() {}

protected:
        /// Destructor has to be virtual.
        ~WorkplaceTest() override {}

        /// Set up for the test fixture
        void SetUp() override {}

        /// Tearing down the test fixture
        void TearDown() override {}

private:
};

TEST_P(WorkplaceTest, HappyDayScenario)
{
        // -----------------------------------------------------------------------------------------
        // Initialize the GeoGrid.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid." << endl;
        GeoGrid grid("config/geogen_default.xml");
        cout << "Done building the GeoGrid." << endl;

        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        // Testing 10 randomly chosen cities atm instead of testing all the 327 cities

        /*
        /*unsigned int leuven       = 877;
        unsigned int brugge       = 961;
        unsigned int roeselare    = 471;
        unsigned int aalst        = 663;
        unsigned int dendermonde  = 371;
        unsigned int temse        = 213;
        unsigned int sinttruiden  = 294;
        unsigned int tongeren     = 210;
        unsigned int maasmechelen = 216;

        ASSERT_NO_FATAL_FAILURE(grid.GenerateWorkplaces());

        auto cities = grid.GetCities();


        double possible_workers_count = cities.at(11002).GetPopulation() *
                (grid.GetFraction(Fractions::YOUNG_WORKERS) + grid.GetFraction(Fractions::YOUNG_WORKERS));
        double workers = grid.GetFraction(Fractions::ACTIVE) *
        unsigned int antwerpen = ;
        */

        EXPECT_EQ(cities.at(11002).GetWorkplaces().size(), antwerpen);
        /*EXPECT_EQ(cities.at(24062).GetWorkplaces().size(), leuven);
        EXPECT_EQ(cities.at(31005).GetWorkplaces().size(), brugge);
        EXPECT_EQ(cities.at(36015).GetWorkplaces().size(), roeselare);
        EXPECT_EQ(cities.at(41002).GetWorkplaces().size(), aalst);
        EXPECT_EQ(cities.at(42006).GetWorkplaces().size(), dendermonde);
        EXPECT_EQ(cities.at(46025).GetWorkplaces().size(), temse);
        EXPECT_EQ(cities.at(71053).GetWorkplaces().size(), sinttruiden);
        EXPECT_EQ(cities.at(73083).GetWorkplaces().size(), tongeren);
        EXPECT_EQ(cities.at(73107).GetWorkplaces().size(), maasmechelen);
         */

}

TEST_P(WorkplaceTest, CommuterVsLocal)
{
        // -----------------------------------------------------------------------------------------
        // Initialize the GeoGrid.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid." << endl;
        GeoGrid grid("config/geogen_default.xml");
        cout << "Done building the GeoGrid." << endl;

        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        auto         cities       = grid.GetCities();
        unsigned int random_city1 = 23096;
        unsigned int random_city2 = 36008;

        // setting local_commuters and no commuters from other cities
        for (auto city : cities) {
                cities.at(random_city1).SetInCommuters(city.first, 0);
                cities.at(random_city2).SetInCommuters(city.first, 0);
        }

        cities.at(random_city1).SetInCommuters(random_city1, 200);

        // the other city will have commuters from other cities but no locals
        cities.at(random_city2).SetInCommuters(35014, 100);
        cities.at(random_city2).SetInCommuters(35029, 50);
        cities.at(random_city2).SetInCommuters(37002, 50);

        ASSERT_NO_FATAL_FAILURE(grid.GenerateWorkplaces());

        // commenting this out until we verify that GenerateWorkplaces was implemented correctly
        EXPECT_EQ(cities.at(random_city1).GetWorkplaces().size(), cities.at(random_city2).GetWorkplaces().size());
}

TEST_P(WorkplaceTest, Extremeregion)
{
        // -----------------------------------------------------------------------------------------
        // Initialize the GeoGrid.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid." << endl;
        GeoGrid grid("config/geogen_default.xml");
        cout << "Done building the GeoGrid." << endl;

        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        auto     cities        = grid.GetCities();
        unsigned a_random_city = 11004;

        for (auto city : cities) {
                cities.at(a_random_city).SetInCommuters(city.first, 0);
        }

        ASSERT_NO_FATAL_FAILURE(grid.GenerateWorkplaces());

        // This is an extreme city where everybody commutes to other cities to work
        EXPECT_EQ(0, cities.at(a_random_city).GetWorkplaces().size());
}
namespace {
// OpenMP should have no effect atm...
#ifdef _OPENMP
unsigned int threads[]{1U, 4U};
#else
unsigned int threads[]{1U};
#endif

} // namespace

INSTANTIATE_TEST_CASE_P(Run, WorkplaceTest, ValuesIn(threads));

} // namespace Tests
