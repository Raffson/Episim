//
// Created by beau on 3/15/18.
//

/**
 * @file
 * Implementation of tests to check the generation of communities.
 */

#include "popgen-Episim/generator/GeoGridGenerator.h"

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
        ~WorkplaceTest() = default;

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
        shared_ptr<GeoGrid> grid = GeoGridGenerator().Generate("run_default.xml");
        cout << "Done building the GeoGrid." << endl;

        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        double possibleWorkers =  grid->GetFraction(Fractions::YOUNG) * (1 - grid->GetFraction(Fractions::STUDENTS))
                               + grid->GetFraction(Fractions::MIDDLE_AGED);
        double activePop = possibleWorkers * grid->GetFraction(Fractions::ACTIVE);
        double allworkers           = activePop * grid->GetTotalPopOfModel();
        double numberOfWorkplaces = ceil(allworkers / grid->GetAvgSize(Sizes::WORKPLACES));

        double margin = 0.01;

        for(auto& it: grid->GetCities()){
            City* aCity = &it.second;
            double inCommutersModifier = (aCity->HasCollege()) ? possibleWorkers : 1.0;
            double workers = aCity->GetPopulation() * activePop +
                             aCity->GetTotalInCommutersCount() * inCommutersModifier -
                             aCity->GetTotalOutCommutersCount() * possibleWorkers;
            double target = workers / allworkers;
            double actual = aCity->GetWorkplaces().size() / numberOfWorkplaces;
            EXPECT_NEAR(actual, target, margin);
            //ceil helps when there is a city with no workplace (eg-HERSTAPPE)
        }

}

/* this test is no longer compatible with the way we do things, either fix or remove this
TEST_P(WorkplaceTest, CommuterVsLocal)
{
        // -----------------------------------------------------------------------------------------
        // Initialize the GeoGrid.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid." << endl;
        shared_ptr<GeoGrid> grid = GeoGridGenerator().Generate("run_default.xml");
        cout << "Done building the GeoGrid." << endl;

        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        auto         cities       = grid->GetCities();
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

        //ASSERT_NO_FATAL_FAILURE(grid->GenerateWorkplaces()); //no longer possible...

        // commenting this out until we verify that GenerateWorkplaces was implemented correctly
        //EXPECT_EQ(cities.at(random_city1).GetWorkplaces().size(), cities.at(random_city2).GetWorkplaces().size());
}
*/

/* this test is no longer compatible with the way we do things, either fix or remove this
TEST_P(WorkplaceTest, Extremeregion)
{
        // -----------------------------------------------------------------------------------------
        // Initialize the GeoGrid.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid." << endl;
        shared_ptr<GeoGrid> grid = GeoGridGenerator().Generate("run_default.xml");
        cout << "Done building the GeoGrid." << endl;

        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        auto     cities        = grid->GetCities();
        unsigned a_random_city = 11004;

        for (auto city : cities) {
                cities.at(a_random_city).SetInCommuters(city.first, 0);
        }

        //ASSERT_NO_FATAL_FAILURE(grid->GenerateWorkplaces()); //no longer possible...

        // This is an extreme city where everybody commutes to other cities to work
        //EXPECT_EQ(0, cities.at(a_random_city).GetWorkplaces().size());
}
*/

namespace {
// OpenMP should have no effect atm...
#ifdef _OPENMP
unsigned int threads[]{1U};
#else
unsigned int threads[]{1U};
#endif

} // namespace

INSTANTIATE_TEST_CASE_P(Run, WorkplaceTest, ValuesIn(threads));

} // namespace Tests
