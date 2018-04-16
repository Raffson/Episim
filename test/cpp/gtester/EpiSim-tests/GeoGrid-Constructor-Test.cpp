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
using namespace geogen;

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

        ASSERT_NO_THROW(GeoGrid("config/geogen_default.xml"));

        // -----------------------------------------------------------------------------------------
        // Initialize the GeoGrid.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid." << endl;
        GeoGrid grid("config/geogen_default.xml");
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
        EXPECT_FLOAT_EQ(grid.GetFraction(Fractions::SCHOOLED), 0.0);
        EXPECT_FLOAT_EQ(grid.GetFraction(Fractions::YOUNG_WORKERS), 0.0);
        EXPECT_FLOAT_EQ(grid.GetFraction(Fractions::OLD_WORKERS), 0.0);
        EXPECT_FLOAT_EQ(grid.GetFraction(Fractions::TODDLERS), 0.0);
        EXPECT_FLOAT_EQ(grid.GetFraction(Fractions::OLDIES), 0.0);
        EXPECT_FLOAT_EQ(grid.GetFraction(Fractions::STUDENTS), 0.0);
        EXPECT_FLOAT_EQ(grid.GetFraction(Fractions::COMMUTING_STUDENTS), 0.0);
        EXPECT_FLOAT_EQ(grid.GetFraction(Fractions::ACTIVE), 0.0);
        EXPECT_FLOAT_EQ(grid.GetFraction(Fractions::COMMUTING_WORKERS), 0.0);
        EXPECT_EQ(grid.GetAvgSize(Sizes::AVERAGE_CP), 0);
        EXPECT_EQ(grid.GetAvgSize(Sizes::SCHOOLS), 0);
        EXPECT_EQ(grid.GetAvgSize(Sizes::COLLEGES), 0);
        EXPECT_EQ(grid.GetAvgSize(Sizes::MAXLC), 0);
        EXPECT_EQ(grid.GetAvgSize(Sizes::COMMUNITIES), 0);
        EXPECT_EQ(grid.GetAvgSize(Sizes::WORKPLACES), 0);
        EXPECT_EQ(grid.GetPopulation().size(), 0);
}

TEST(GeoGridCtorTest, NonExistingFile)
{
        ::testing::FLAGS_gtest_death_test_style = "threadsafe";
        // Test with a non-existing file
        ASSERT_DEATH_IF_SUPPORTED(GeoGrid("bad input"), "");
}

TEST(GeoGridCtorTest, BadFractions)
{
        ::testing::FLAGS_gtest_death_test_style = "threadsafe";
        // Test with a non-existing file
        ASSERT_DEATH_IF_SUPPORTED(GeoGrid("config/bad_student_frac_0.xml"), "");
        ASSERT_DEATH_IF_SUPPORTED(GeoGrid("config/bad_stucom_frac_0.xml"), "");
        ASSERT_DEATH_IF_SUPPORTED(GeoGrid("config/bad_active_frac_0.xml"), "");
        ASSERT_DEATH_IF_SUPPORTED(GeoGrid("config/bad_workcom_frac_0.xml"), "");
        ASSERT_DEATH_IF_SUPPORTED(GeoGrid("config/bad_student_frac_1.xml"), "");
        ASSERT_DEATH_IF_SUPPORTED(GeoGrid("config/bad_stucom_frac_1.xml"), "");
        ASSERT_DEATH_IF_SUPPORTED(GeoGrid("config/bad_active_frac_1.xml"), "");
        ASSERT_DEATH_IF_SUPPORTED(GeoGrid("config/bad_workcom_frac_1.xml"), "");

        // This last test will check a contactpool's size that exceeds the cap op 1000...
        ASSERT_DEATH_IF_SUPPORTED(GeoGrid("config/bad_contactpool_size.xml"), "");
}

TEST(GeoGridCtorTest, FaultyCityRow)
{

        // -----------------------------------------------------------------------------------------
        // Parse cities.
        // -----------------------------------------------------------------------------------------
        cout << "Parsing cities." << endl;
        map<unsigned int, City> cty_map;
        parser::ParseCities("data/flanders_cities_faulty_row.csv", "data/flanders_commuting.csv", cty_map, false);
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
        parser::ParseCities("data/flanders_cities_faulty_col.csv", "data/flanders_commuting.csv", cty_map, false);
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
        GeoGrid grid("config/geogen_default.xml");
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
