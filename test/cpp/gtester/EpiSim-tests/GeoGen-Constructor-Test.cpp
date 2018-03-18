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

    class GeoGenCtorTest : public ::testing::TestWithParam<unsigned int>
    {
    public:
        /// TestCase set up.
        static void SetUpTestCase() {}

        /// Tearing down TestCase
        static void TearDownTestCase() {}


    protected:
        /// Destructor has to be virtual.
        ~GeoGenCtorTest() override {}

        /// Set up for the test fixture
        void SetUp() override {}

        /// Tearing down the test fixture
        void TearDown() override {}
    };

    TEST(GeoGenCtorTest, HappyDayScenario){

        ASSERT_NO_THROW(GeoGrid("config/geogen_default.xml"));

        // -----------------------------------------------------------------------------------------
        // Initialize the GeoGrid.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid." << endl;
        auto grid = GeoGrid("config/geogen_default.xml");
        cout << "Done building the GeoGrid." << endl;


        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        map<int, shared_ptr<City>> mp = grid.get_cities();

        shared_ptr<City> antwerp = mp[11002];
        Coordinate coord = antwerp->GetCoordinates();

        EXPECT_EQ(1, antwerp->GetProvince());
        EXPECT_EQ(269954, antwerp->GetPopulation());
        EXPECT_FLOAT_EQ(153104.586, coord.x);
        EXPECT_FLOAT_EQ(212271.7101, coord.y);
        EXPECT_FLOAT_EQ(51.2165845, coord.latitude);
        EXPECT_FLOAT_EQ(4.413545489, coord.longitude);
        EXPECT_EQ("ANTWERPEN", antwerp->GetName());

    }

    TEST(GeoGenCtorTest, FaultyCityRow){

        // -----------------------------------------------------------------------------------------
        // Parse cities.
        // -----------------------------------------------------------------------------------------
        cout << "Parsing cities." << endl;
        map<int, shared_ptr<City>> cty_map = parser::ParseCities("data/flanders_cities_faulty_row.csv", "data/flanders_commuting.csv", false);
        cout << "Done parsing cities." << endl;


        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        EXPECT_EQ(326, cty_map.size());
    }

    TEST(GeoGenCtorTest, FaultyCityColumn){

        // -----------------------------------------------------------------------------------------
        // Parse cities.
        // -----------------------------------------------------------------------------------------
        cout << "Parsing cities." << endl;
        map<int, shared_ptr<City>> cty_map = parser::ParseCities("data/flanders_cities_faulty_col.csv", "data/flanders_commuting.csv", false);
        cout << "Done parsing cities." << endl;


        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        EXPECT_EQ(327, cty_map.size());

        shared_ptr<City> antwerp = cty_map[11002];
        Coordinate coord = antwerp->GetCoordinates();

        EXPECT_EQ(1, antwerp->GetProvince());
        EXPECT_EQ(269954, antwerp->GetPopulation());

        EXPECT_FLOAT_EQ(153104.586, coord.x);
        EXPECT_FLOAT_EQ(212271.7101, coord.y);
        EXPECT_FLOAT_EQ(51.2165845, coord.latitude);
        EXPECT_FLOAT_EQ(4.413545489, coord.longitude);
        EXPECT_EQ("ANTWERPEN", antwerp->GetName());

    }

    TEST(GeoGenCtorTest, CityRowCounter){

        // -----------------------------------------------------------------------------------------
        // Initialize the GeoGrid.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid." << endl;
        auto grid = GeoGrid("config/geogen_default.xml");
        cout << "Done building the GeoGrid." << endl;


        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        EXPECT_EQ(327, grid.get_cities().size());

    }

    namespace {
//OpenMP should have no effect atm...
#ifdef _OPENMP
    unsigned int threads[]{1U, 4U};
#else
    unsigned int threads[]{1U};
#endif

    } // namespace

    INSTANTIATE_TEST_CASE_P(Run, GeoGenCtorTest, ValuesIn(threads));

} // namespace Tests
