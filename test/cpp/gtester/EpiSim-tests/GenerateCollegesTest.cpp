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

namespace Tests {

    using namespace std;
    using namespace ::testing;
    using namespace geogen;

    using boost::property_tree::ptree;

    class CollegeTest : public ::testing::TestWithParam<tuple<const char*, unsigned int>>
    {
    public:
        /// TestCase set up.
        static void SetUpTestCase() {}

        /// Tearing down TestCase
        static void TearDownTestCase() {}


    protected:
        /// Destructor has to be virtual.
        ~CollegeTest() override {}

        /// Set up for the test fixture
        void SetUp() override {}

        /// Tearing down the test fixture
        void TearDown() override {}
    };

    TEST_P(CollegeTest, Run)
    {

        // -------------------------------------------------------------------------------------
        // Initialize the logger.
        // -------------------------------------------------------------------------------------
        spdlog::set_async_mode(1048576);
        auto file_logger = spdlog::rotating_logger_mt("contact_logger", "test_logfile", numeric_limits<size_t>::max(),
                                                      numeric_limits<size_t>::max());
        file_logger->set_pattern("%v"); // Remove meta data from log => time-stamp of logging

        // -----------------------------------------------------------------------------------------
        // Initialize the simulator.
        // -----------------------------------------------------------------------------------------
        cout << "Building the simulator. " << endl;
        auto grid = GeoGrid("config/geogen_default.xml");
        cout << "Done building the simulator" << endl;

        // -----------------------------------------------------------------------------------------
        // Run the simulation and release logger.
        // -----------------------------------------------------------------------------------------

        spdlog::drop_all();

        // -----------------------------------------------------------------------------------------
        // Check resuts against target number.
        // -----------------------------------------------------------------------------------------
    }

    namespace {
        const char* tags[] = {"r0_0", "r0_4", "r0_8", "r0_12", "r0_16"};

#ifdef _OPENMP
        unsigned int threads[]{1U, 4U};
#else
        unsigned int threads[]{1U};
#endif

    } // namespace

    INSTANTIATE_TEST_CASE_P(Run, CollegeTest, Combine(ValuesIn(tags), ValuesIn(threads)));

} // namespace Tests


