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
        GeoGrid grid;
        grid.Initialize("config/geogen_default.xml");
        cout << "Done building the GeoGrid." << endl;

        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        ASSERT_NO_FATAL_FAILURE(grid.GenerateColleges()); //Must generate colleges to keep track of commuters
        ASSERT_NO_FATAL_FAILURE(grid.GenerateWorkplaces());

        float        margin       = 0.4;

        double possible_workers =  grid.GetFraction(Fractions::YOUNG) * (1 - grid.GetFraction(Fractions::STUDENTS))
                               + grid.GetFraction(Fractions::MIDDLE_AGED);
        double active_pop = possible_workers * grid.GetFraction(Fractions::ACTIVE);


        auto cities = grid.GetCities();
        for(auto& it: cities){
            City* a_city = &it.second;
            double in_commuters_modifier = (a_city->HasCollege()) ? possible_workers : 1.0;
            double workers = a_city->GetPopulation() * active_pop +
                             a_city->GetTotalInCommutersCount() * in_commuters_modifier -
                             a_city->GetTotalOutCommutersCount() * possible_workers;
            auto target_workplaces = workers / grid.GetAvgSize(Sizes::WORKPLACES);
            EXPECT_NEAR(a_city->GetWorkplaces().size(), target_workplaces, ceil(target_workplaces*margin));
            //ceil helps when there is a city with no workplace (eg-HERSTAPPE)
        }

}

TEST_P(WorkplaceTest, CommuterVsLocal)
{
        // -----------------------------------------------------------------------------------------
        // Initialize the GeoGrid.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid." << endl;
        GeoGrid grid;
        grid.Initialize("config/geogen_default.xml");
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
        GeoGrid grid;
        grid.Initialize("config/geogen_default.xml");
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
unsigned int threads[]{1U};
#else
unsigned int threads[]{1U};
#endif

} // namespace

INSTANTIATE_TEST_CASE_P(Run, WorkplaceTest, ValuesIn(threads));

} // namespace Tests
