//
// Created by beau on 3/15/18.
//

/**
 * @file
 * Implementation of tests to check the generation of communities.
 */

#include "popgen-Episim/GeoGen/GeoGrid.h"

#include <boost/property_tree/ptree.hpp>
#include <exception>
#include <gtest/gtest.h>
#include <omp.h>
#include <spdlog/spdlog.h>

namespace Tests {

using namespace std;
using namespace ::testing;
using namespace geogen;

using boost::property_tree::ptree;

class CommunityTest : public ::testing::TestWithParam<unsigned int>
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
        // -----------------------------------------------------------------------------------------
        // Initialize the GeoGrid.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid." << endl;
        auto grid = GeoGrid("config/geogen_default.xml");
        cout << "Done building the GeoGrid." << endl;

        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        auto                                 cities = grid.GetCities();
        map<int, shared_ptr<City>>::iterator c_it2  = cities.begin();
        /// Check if the communities are ditributed correctly.
        for (map<int, shared_ptr<City>>::iterator c_it = cities.begin(); c_it != cities.end(); c_it++) {
                try {
                        if (c_it != cities.end()) {
                                c_it2 = c_it++;
                                c_it--;
                        } else {
                                c_it2 = cities.begin();
                        }
                        /// c_it.first is the ID of the city, c_it.second is a pointer to the city itself.
                        if ((*c_it).first != (*c_it2).first) {
                                if ((*c_it).second->GetPopulation() >= (*c_it2).second->GetPopulation()) {
                                        EXPECT_GE((*c_it).second->GetCommunitySize(),
                                                  (*c_it2).second->GetCommunitySize());
                                }
                                if ((*c_it).second->GetPopulation() <= (*c_it2).second->GetPopulation()) {
                                        EXPECT_LE((*c_it).second->GetCommunitySize(),
                                                  (*c_it2).second->GetCommunitySize());
                                }
                        }
                } catch (std::exception& e) {
                        cout << "error caught: " << e.what() << endl;
                }
        }
}

namespace {
// OpenMP should have no effect atm...
#ifdef _OPENMP
unsigned int threads[]{1U, 4U};
#else
unsigned int threads[]{1U};
#endif

} // namespace

INSTANTIATE_TEST_CASE_P(Run, CommunityTest, ValuesIn(threads));

} // namespace Tests
