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
#include <stdlib.h>

namespace Tests {

using namespace std;
using namespace ::testing;
using namespace stride;

using boost::property_tree::ptree;

class SchoolTest : public ::testing::TestWithParam<unsigned int>
{
public:
        /// TestCase set up.
        static void SetUpTestCase() {}

        /// Tearing down TestCase
        static void TearDownTestCase() {}

protected:
        /// Destructor has to be virtual.
        ~SchoolTest() override {}

        /// Set up for the test fixture
        void SetUp() override {}

        /// Tearing down the test fixture
        void TearDown() override {}
};

TEST_P(SchoolTest, HappyDayScenario)
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

        unsigned int target =
            ceil(grid->GetTotalPop() * grid->GetFraction(Fractions::SCHOOLED) / grid->GetAvgSize(Sizes::SCHOOLS));
        EXPECT_EQ(grid->GetSchoolCount(), target);

        double margin = 0.1;

        for(auto& it:grid->GetCities()){
            City* aCity = &it.second;
            double target = (double)aCity->GetPopulation() / grid->GetTotalPopOfModel();
            double actual = (double)aCity->GetSchools().size() / grid->GetSchoolCount();
            EXPECT_NEAR(actual, target, margin);
        }


}

TEST_P(SchoolTest, HighMoreLowLess)
{
        // -----------------------------------------------------------------------------------------
        // Initialize the GeoGrid.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid." << endl;
        shared_ptr<GeoGrid> grid = GeoGridGenerator().Generate("run_default.xml");
        cout << "Done building the GeoGrid." << endl;

        auto mp = grid->GetCities();

        City*         highestPopC = &mp.begin()->second;
        unsigned long highestPop   = highestPopC->GetAllCommunities().size();

        City*         lowestPopC = &mp.begin()->second;
        unsigned long lowestPop   = lowestPopC->GetAllCommunities().size();

        for (auto& it : mp) {
                City*        tCity = &it.second;
                unsigned int pop    = tCity->GetPopulation();

                if (highestPop < pop) {
                        highestPop   = pop;
                        highestPopC = tCity;
                }

                if (lowestPop > pop) {
                        lowestPop   = pop;
                        lowestPopC = tCity;
                }
        }

        EXPECT_LE(lowestPopC->GetAllCommunities().size(), highestPopC->GetAllCommunities().size());
}

namespace {
// OpenMP should have no effect atm...
#ifdef _OPENMP
unsigned int threads[]{1U};
#else
unsigned int threads[]{1U};
#endif

} // namespace

INSTANTIATE_TEST_CASE_P(Run, SchoolTest, ValuesIn(threads));

} // namespace Tests
