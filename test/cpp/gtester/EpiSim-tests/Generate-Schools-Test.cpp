//
// Created by beau on 3/15/18.
//

/**
 * @file
 * Implementation of tests to check the generation of communities.
 */

#include "popgen-Episim/GeoGen/GeoGrid.h"

#include <gtest/gtest.h>
#include <omp.h>
#include <spdlog/spdlog.h>
#include <boost/property_tree/ptree.hpp>
#include <stdlib.h>

namespace Tests {

    using namespace std;
    using namespace ::testing;
    using namespace geogen;

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
        auto grid = GeoGrid("config/geogen_default.xml");
        cout << "Done building the GeoGrid." << endl;


        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

	    //Do the test...
	    ASSERT_NO_THROW(grid.generate_schools()); // happy day
        EXPECT_EQ(grid.getSchool_count(),1736);

    }

    TEST_P(SchoolTest, WrongInput){
        // -----------------------------------------------------------------------------------------
        // Initialize the GeoGrid.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid." << endl;
        auto grid = GeoGrid("config/geogen_test_school.xml");
        cout << "Done building the GeoGrid." << endl;

        //ASSERT_DEATH_IF_SUPPORTED(grid.generate_schools(), ".");
    }

    namespace {
//OpenMP should have no effect atm...
#ifdef _OPENMP
        unsigned int threads[]{1U, 4U};
#else
        unsigned int threads[]{1U};
#endif

    } // namespace

    INSTANTIATE_TEST_CASE_P(Run, SchoolTest, ValuesIn(threads));

} // namespace Tests


