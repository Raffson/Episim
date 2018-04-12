//
// Created by robbe on 12/04/18.
//

#include <vector>
#include "popgen-Episim/GeoGen/GeoGrid.h"

#include <gtest/gtest.h>


namespace Tests {

    using namespace std;
    using namespace ::testing;
    using namespace geogen;

    using boost::property_tree::ptree;

    class DefragCityTest : public ::testing::TestWithParam<unsigned int> {
    public:
        DefragCityTest() = default;

    protected:
        GeoGrid* geo{};
        /// Destructor has to be virtual.
        ~DefragCityTest() override = default;

        /// Set up for the test fixture
        void SetUp() override {
        geo = new GeoGrid("config/geogen_default.xml");
        }

        /// Tearing down the test fixture
        void TearDown() override {
                    delete geo;
        }
    };

    TEST_P(DefragCityTest, happy_defrag) {
        double X = 0.5;
        double Y = 0.001; // should be 32 cities
        vector<double> p_vec = {1};

        long counter = 0;
        double pop_treshold = X * geo->GetTotalPop();
        geo->GenerateAll();
        for(auto &it: geo->GetCities()){ //327 cities

            if (pop_treshold > it.second.GetPopulation()){
                counter++;
            }
        }

        geo->DefragmentSmallestCities(X, Y, p_vec);

        EXPECT_EQ(343, geo->GetCities().size());


    }

    namespace {
// OpenMP should have no effect atm...
    #ifdef _OPENMP
            unsigned int threads[]{1U, 4U};
    #else
            unsigned int threads[]{1U};
    #endif

    } // namespace

    INSTANTIATE_TEST_CASE_P(Run, DefragCityTest,ValuesIn(threads));
}// Namespace Test


