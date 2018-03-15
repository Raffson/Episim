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

    class CommunityTest : public ::testing::TestWithParam<tuple<const char*, unsigned int>>
    {
    public:
        /// TestCase set up.
        static void SetUpTestCase() {}

        /// Tearing down TestCase
        static void TearDownTestCase() {}


    protected:
        /// Destructor has to be virtual.
        ~CommunityTest() override {}

        /// Set up for the test fixture
        void SetUp() override {}

        /// Tearing down the test fixture
        void TearDown() override {}
    };

    TEST_P(CommunityTest, Run)
    {

        // -------------------------------------------------------------------------------------
        // Initialize the logger.
        // -------------------------------------------------------------------------------------
//        spdlog::set_async_mode(numeric_limits<unsigned int>::max());
//        auto file_logger = spdlog::rotating_logger_mt("contact_logger", "test_logfile", numeric_limits<size_t>::max(),
//                                                      numeric_limits<size_t>::max());
//        file_logger->set_pattern("%v"); // Remove meta data from log => time-stamp of logging

        // -----------------------------------------------------------------------------------------
        // Initialize the simulator.
        // -----------------------------------------------------------------------------------------
        cout << "Building the simulator. " << endl;
        auto grid = GeoGrid("config/geogen_default.xml");
        cout << "Done building the simulator" << endl;

        // -----------------------------------------------------------------------------------------
        // Run the simulation and release logger.
        // -----------------------------------------------------------------------------------------

//        spdlog::drop_all();

        // -----------------------------------------------------------------------------------------
        // Check resuts against target number.
        // -----------------------------------------------------------------------------------------

        map<int, shared_ptr<City>> cities = grid.get_cities();
        /// Check if the communities are ditributed correctly.
        for (map<int, shared_ptr<City>>::iterator c_it = cities.begin(); c_it != cities.end(); ++c_it){
            map<int, shared_ptr<City>>::iterator c_it2 = cities.begin();
            if (c_it != cities.end()){
                c_it2 = c_it++;
            }
            /// c_it.first is the ID of the city, c_it.second is a pointer to the city itself.
            if ((*c_it).first != (*c_it2).first) {
                if ((*c_it).second->getPopulation() >= (*c_it2).second->getPopulation()) {
                    EXPECT_GE((*c_it).second->getCommunitySize(), (*c_it2).second->getCommunitySize());
                }
                if ((*c_it).second->getPopulation() <= (*c_it2).second->getPopulation()) {
                    EXPECT_LE((*c_it).second->getCommunitySize(), (*c_it2).second->getCommunitySize());
                }
            }
        }

    }

    namespace {
        const char* tags[] = {"r0_0", "r0_4", "r0_8", "r0_12", "r0_16"};

#ifdef _OPENMP
        unsigned int threads[]{1U, 4U};
#else
        unsigned int threads[]{1U};
#endif

    } // namespace

    INSTANTIATE_TEST_CASE_P(Run, CommunityTest, Combine(ValuesIn(tags), ValuesIn(threads)));

} // namespace Tests


