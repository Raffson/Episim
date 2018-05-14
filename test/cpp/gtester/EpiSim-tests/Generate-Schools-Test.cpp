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
        GeoGrid grid;
        ASSERT_NO_FATAL_FAILURE(grid.Initialize("run_default.xml"));
        cout << "Done building the GeoGrid." << endl;

        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        // Do the test...
        ASSERT_NO_FATAL_FAILURE(grid.GenerateColleges());
        ASSERT_NO_FATAL_FAILURE(grid.GenerateWorkplaces());
        ASSERT_NO_FATAL_FAILURE(grid.GenerateSchools()); // happy day
        unsigned int target =
            ceil(grid.GetTotalPop() * grid.GetFraction(Fractions::SCHOOLED) / grid.GetAvgSize(Sizes::SCHOOLS));
        EXPECT_EQ(grid.GetSchoolCount(), target);

        double margin = 0.01;

        for(auto& it:grid.GetCities()){
            City* a_city = &it.second;
            double target = (double)a_city->GetPopulation() / grid.GetTotalPopOfModel();
            double actual = (double)a_city->GetSchools().size() / grid.GetSchoolCount();
            EXPECT_NEAR(actual, target, margin);
        }


}

TEST_P(SchoolTest, WrongInput)
{
        // -----------------------------------------------------------------------------------------
        // Initialize the GeoGrid.
        // -----------------------------------------------------------------------------------------

        /* After reworking the fractions (deduced from households) these death tests won't die anymore...
         * need some new relevant tests here...
         * currently i'm leaving this in comments, as well as leaving the xml files in place
         * this (code below + files) should be deleted in the future (or reworked)
         *
        // Bad input file with fractions that are FUBAR
        auto grid = GeoGrid("config/schools&colleges_bad_frac_0.xml");
        ASSERT_DEATH_IF_SUPPORTED(grid.GenerateSchools(), "");
        grid = GeoGrid("config/schools&colleges_bad_frac_1.xml");
        ASSERT_DEATH_IF_SUPPORTED(grid.GenerateSchools(), "");
        */

        // uncomment the next 2 lines once the "unsigned int refractor" has happened...
        // grid = GeoGrid("config/bad_community_sizes.xml");
        // ASSERT_DEATH_IF_SUPPORTED(grid.GenerateSchools(), "");
}

TEST_P(SchoolTest, HighMoreLowLess)
{
        // -----------------------------------------------------------------------------------------
        // Initialize the GeoGrid.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid." << endl;
        GeoGrid grid;
        ASSERT_NO_FATAL_FAILURE(grid.Initialize("run_default.xml"));
        cout << "Done building the GeoGrid." << endl;

        auto mp = grid.GetCities();

        City*         highest_pop_c = &mp.begin()->second;
        unsigned long highest_pop   = highest_pop_c->GetAllCommunities().size();

        City*         lowest_pop_c = &mp.begin()->second;
        unsigned long lowest_pop   = lowest_pop_c->GetAllCommunities().size();

        for (auto& it : mp) {
                City*        t_city = &it.second;
                unsigned int pop    = t_city->GetPopulation();

                if (highest_pop < pop) {
                        highest_pop   = pop;
                        highest_pop_c = t_city;
                }

                if (lowest_pop > pop) {
                        lowest_pop   = pop;
                        lowest_pop_c = t_city;
                }
        }

        EXPECT_LE(lowest_pop_c->GetAllCommunities().size(), highest_pop_c->GetAllCommunities().size());
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
