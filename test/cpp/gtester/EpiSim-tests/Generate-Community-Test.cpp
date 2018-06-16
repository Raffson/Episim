//
// Created by beau on 3/15/18.
//

/**
 * @file
 * Implementation of tests to check the generation of communities.
 */

#include "popgen-Episim/generator/GeoGridGenerator.h"

#include <boost/property_tree/ptree.hpp>
#include <exception>
#include <gtest/gtest.h>
#include <omp.h>
#include <spdlog/spdlog.h>

namespace Tests {

using namespace std;
using namespace ::testing;
using namespace stride;

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
    TEST_P(CommunityTest, HappyDayScenario)
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

            // Do the test...

            //times 2 because we have to count both Primary and Secondary communities
            double totalCommunities = 2 * ceil((double)grid->GetTotalPop() / grid->GetAvgSize(Sizes::COMMUNITIES));
            double margin = 0.01;

            for(auto& it: grid->GetCities()){
                    City* aCity = &it.second;
                    double target = (double)aCity->GetPopulation() / grid->GetTotalPopOfModel();
                    double actual = (aCity->GetPrimaryCommunities().size() + aCity->GetSecondaryCommunities().size())
                                    / totalCommunities;

                    EXPECT_NEAR(actual, target, margin);
            }


    }
TEST_P(CommunityTest, Run)
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

        auto                              cities = grid->GetCities();
        map<unsigned int, City>::iterator cIt2  = cities.begin();
        /// Check if the communities are ditributed correctly.
        for (map<unsigned int, City>::iterator cIt = cities.begin(); cIt != cities.end(); cIt++) {
                try {
                        if (cIt != cities.end()) {
                                cIt2 = cIt++;
                                cIt--;
                        } else {
                                cIt2 = cities.begin();
                        }
                        /// c_it.first is the ID of the city, c_it.second is a pointer to the city itself.
                        if ((*cIt).first != (*cIt2).first) {
                                if ((*cIt).second.GetPopulation() >= (*cIt2).second.GetPopulation()) {
                                        EXPECT_GE((*cIt).second.GetNrOfCommunities(),
                                                  (*cIt2).second.GetNrOfCommunities());
                                }
                                if ((*cIt).second.GetPopulation() <= (*cIt2).second.GetPopulation()) {
                                        EXPECT_LE((*cIt).second.GetNrOfCommunities(),
                                                  (*cIt2).second.GetNrOfCommunities());
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
unsigned int threads[]{1U};
#else
unsigned int threads[]{1U};
#endif

} // namespace

INSTANTIATE_TEST_CASE_P(Run, CommunityTest, ValuesIn(threads));

} // namespace Tests
