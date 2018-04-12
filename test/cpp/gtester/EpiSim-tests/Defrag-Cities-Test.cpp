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
        double X = 0.5; // We will fragment half of the chosen cities
        double Y = 0.001; // should be 32 cities
        vector<double> p_vec = {1};
        geo->DefragmentSmallestCities(X, Y, p_vec);
        // 327 cities in basic config, 32 cities up for fragmentation, we fragment half.
        // 327 + (32 * 0.5) = 343
        EXPECT_FLOAT_EQ(343, geo->GetCities().size());

    }

    TEST_P(DefragCityTest, value_test){


        City* smallest_city = &geo->GetCities().begin()->second;

        for(auto &it: geo->GetCities()){

            if(smallest_city->GetPopulation() > it.second.GetPopulation()){
                smallest_city = &it.second;
            }
        }

        double Y = ((double) smallest_city->GetPopulation() /(double) geo->GetTotalPop());
        double X = 1.0;
        vector<double> p_vec = {1};

        Coordinate old_coordinate = smallest_city->GetCoordinates();
        string old_name =  smallest_city->GetName();
        unsigned int old_province = smallest_city->GetProvince();
        unsigned int old_last_id = geo->GetCities().rbegin()->second.GetId();
        geo->DefragmentSmallestCities(X, Y, p_vec);

        ASSERT_EQ(328, geo->GetCities().size()); // Test if we only fragmented one city

        City* last_city = &geo->GetCities().rbegin()->second;
        auto r_it = geo->GetCities().rbegin();
        r_it++;
        City* second_last_city = & r_it->second;
        ASSERT_NE(last_city, second_last_city);

        // Splitted in 2 cities. Coordinates of the first city should match the original city
        EXPECT_FLOAT_EQ(old_coordinate.longitude, second_last_city->GetCoordinates().longitude);
        EXPECT_FLOAT_EQ(old_coordinate.latitude, second_last_city->GetCoordinates().latitude);
        EXPECT_FLOAT_EQ(old_coordinate.x, second_last_city->GetCoordinates().x);
        EXPECT_FLOAT_EQ(old_coordinate.y, second_last_city->GetCoordinates().y);

        // Coordinates of the second city are 0.1 off the original
        EXPECT_FLOAT_EQ(old_coordinate.longitude - 0.1,last_city->GetCoordinates().longitude);
        EXPECT_FLOAT_EQ(old_coordinate.latitude - 0.1, last_city->GetCoordinates().latitude);
        EXPECT_FLOAT_EQ(old_coordinate.x - 0.1, last_city->GetCoordinates().x);
        EXPECT_FLOAT_EQ(old_coordinate.y - 0.1, last_city->GetCoordinates().y);

        //Province check, should be the same
        EXPECT_EQ(old_province, second_last_city->GetProvince());
        EXPECT_EQ(old_province, last_city->GetProvince());

        // New ID check
        EXPECT_EQ(old_last_id + 1, second_last_city->GetId());
        EXPECT_EQ(old_last_id + 2, last_city->GetId());

        // New names
        EXPECT_EQ(old_name + "0", second_last_city->GetName());
        EXPECT_EQ(old_name + "1", last_city->GetName());



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


