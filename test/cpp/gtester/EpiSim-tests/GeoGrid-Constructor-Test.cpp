/**
 * @file
 * Implementation of tests to check the generation of communities.
 */

#include "popgen-Episim/GeoGen/GeoGrid.h"

#include <boost/property_tree/ptree.hpp>
#include <gtest/gtest.h>
#include <omp.h>
#include <spdlog/spdlog.h>

namespace Tests {

using namespace std;
using namespace ::testing;
using namespace stride;

using boost::property_tree::ptree;

class GeoGridCtorTest : public ::testing::TestWithParam<unsigned int>
{
public:
        /// TestCase set up.
        static void SetUpTestCase() {}

        /// Tearing down TestCase
        static void TearDownTestCase() {}

protected:
        /// Destructor has to be virtual.
        ~GeoGridCtorTest() override {}

        /// Set up for the test fixture
        void SetUp() override {}

        /// Tearing down the test fixture
        void TearDown() override {}
};

TEST(GeoGridCtorTest, HappyDayScenario)
{
        GeoGrid grid;

        // -----------------------------------------------------------------------------------------
        // Initialize the GeoGrid.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid." << endl;
        ASSERT_NO_THROW(grid.Initialize("config/geogen_default.xml"));
        cout << "Done building the GeoGrid." << endl;

        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        map<unsigned int, City> mp = grid.GetCities();

        City&      antwerp = mp.at(11002);
        Coordinate coord   = antwerp.GetCoordinates();

        EXPECT_EQ(1, antwerp.GetProvince());
        EXPECT_EQ(269954, antwerp.GetPopulation());
        EXPECT_FLOAT_EQ(153104.586, coord.x);
        EXPECT_FLOAT_EQ(212271.7101, coord.y);
        EXPECT_FLOAT_EQ(51.2165845, coord.latitude);
        EXPECT_FLOAT_EQ(4.413545489, coord.longitude);
        EXPECT_EQ("ANTWERPEN", antwerp.GetName());
}

TEST(GeoGridCtorTest, DefaultConstructor)
{
        GeoGrid grid;
        EXPECT_EQ(grid.GetCities().size(), 0);
        EXPECT_EQ(grid.GetTotalPop(), 0);
        for( auto frac : FractionList )
                EXPECT_FLOAT_EQ(grid.GetFraction(frac), 0.0);
        for( auto size : SizeList )
                EXPECT_EQ(grid.GetAvgSize(size), 0);
        EXPECT_EQ(grid.GetPopulation()->size(), 0);
}

TEST(GeoGridCtorTest, NonExistingFile)
{
        ::testing::FLAGS_gtest_death_test_style = "threadsafe";
        // Test with a non-existing file
        GeoGrid grid;
        ASSERT_DEATH_IF_SUPPORTED(grid.Initialize("bad input"), "");
}

TEST(GeoGridCtorTest, BadFractions)
{
        ::testing::FLAGS_gtest_death_test_style = "threadsafe";
        // Test with a non-existing file
        GeoGrid grid;
        ASSERT_DEATH_IF_SUPPORTED(grid.Initialize("config/bad_student_frac_0.xml"), "");
        ASSERT_DEATH_IF_SUPPORTED(grid.Initialize("config/bad_stucom_frac_0.xml"), "");
        ASSERT_DEATH_IF_SUPPORTED(grid.Initialize("config/bad_active_frac_0.xml"), "");
        ASSERT_DEATH_IF_SUPPORTED(grid.Initialize("config/bad_workcom_frac_0.xml"), "");
        ASSERT_DEATH_IF_SUPPORTED(grid.Initialize("config/bad_student_frac_1.xml"), "");
        ASSERT_DEATH_IF_SUPPORTED(grid.Initialize("config/bad_stucom_frac_1.xml"), "");
        ASSERT_DEATH_IF_SUPPORTED(grid.Initialize("config/bad_active_frac_1.xml"), "");
        ASSERT_DEATH_IF_SUPPORTED(grid.Initialize("config/bad_workcom_frac_1.xml"), "");

        // This last test will check a contactpool's size that exceeds the cap op 1000...
        ASSERT_DEATH_IF_SUPPORTED(grid.Initialize("config/bad_contactpool_size.xml"), "");
}

TEST(GeoGridCtorTest, FaultyCityRow)
{

        // -----------------------------------------------------------------------------------------
        // Parse cities.
        // -----------------------------------------------------------------------------------------
        cout << "Parsing cities." << endl;
        map<unsigned int, City> cty_map;
        unsigned int            totpop = 0;
        parser::ParseCities("data/flanders_cities_faulty_row.csv", cty_map, totpop);
        cout << "Done parsing cities." << endl;

        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        EXPECT_EQ(326, cty_map.size());
}

TEST(GeoGridCtorTest, FaultyCityColumn)
{

        // -----------------------------------------------------------------------------------------
        // Parse cities.
        // -----------------------------------------------------------------------------------------
        cout << "Parsing cities." << endl;
        map<unsigned int, City> cty_map;
        unsigned int            totpop = 0;
        parser::ParseCities("data/flanders_cities_faulty_col.csv", cty_map, totpop);
        cout << "Done parsing cities." << endl;

        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        EXPECT_EQ(327, cty_map.size());

        City&      antwerp = cty_map.at(11002);
        Coordinate coord   = antwerp.GetCoordinates();

        EXPECT_EQ(1, antwerp.GetProvince());
        EXPECT_EQ(269954, antwerp.GetPopulation());

        EXPECT_FLOAT_EQ(153104.586, coord.x);
        EXPECT_FLOAT_EQ(212271.7101, coord.y);
        EXPECT_FLOAT_EQ(51.2165845, coord.latitude);
        EXPECT_FLOAT_EQ(4.413545489, coord.longitude);
        EXPECT_EQ("ANTWERPEN", antwerp.GetName());
}

TEST(GeoGridCtorTest, CityRowCounter)
{

        // -----------------------------------------------------------------------------------------
        // Initialize the GeoGrid.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid." << endl;
        GeoGrid grid;
        grid.Initialize("config/geogen_default.xml");
        cout << "Done building the GeoGrid." << endl;

        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        EXPECT_EQ(327, grid.GetCities().size());
}

namespace {
// OpenMP should have no effect atm...
#ifdef _OPENMP
unsigned int threads[]{1U};
#else
unsigned int threads[]{1U};
#endif

} // namespace

INSTANTIATE_TEST_CASE_P(Run, GeoGridCtorTest, ValuesIn(threads));

} // namespace Tests