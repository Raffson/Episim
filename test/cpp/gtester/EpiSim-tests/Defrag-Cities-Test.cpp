//
// Created by robbe on 12/04/18.
//

#include <vector>
#include "popgen-Episim/GeoGen/GeoGrid.h"

#include <boost/property_tree/ptree.hpp>
#include <gtest/gtest.h>
#include <omp.h>
#include <spdlog/spdlog.h>

namespace Tests {

    using namespace std;
    using namespace ::testing;
    using namespace geogen;

    using boost::property_tree::ptree;

    class DefragCityTest : public ::testing::TestWithParam<unsigned int> {
    public:
        /// TestCase set up.
        static void SetUpTestCase() {}

        /// Tearing down TestCase
        static void TearDownTestCase() {}

    protected:
        /// Destructor has to be virtual.
        ~DefragCityTest() override = default;

        /// Set up for the test fixture
        void SetUp() override {}

        /// Tearing down the test fixture
        void TearDown() override {}
    };

    TEST_P(DefragCityTest, happy_defrag) {

        double X = 0.1;
        double Y = 0.5;
        vector<double> p_vec = {0,1};
    }
}// Namespace Test


