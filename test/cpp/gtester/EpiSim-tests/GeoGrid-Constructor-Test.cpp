/**
 * @file
 * Implementation of tests to check the generation of communities.
 */

#include "popgen-Episim/model/GeoGrid.h"

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
        ASSERT_NO_FATAL_FAILURE(grid.Initialize("run_default.xml"));
        cout << "Done building the GeoGrid." << endl;

        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        map<unsigned int, City> mp = grid.GetCities();

        City&      antwerp = mp.at(11002);
        Coordinate coord   = antwerp.GetCoordinates();

        EXPECT_EQ(1, antwerp.GetProvince());
        EXPECT_EQ(269954, antwerp.GetPopulation());
        EXPECT_FLOAT_EQ(153104.586, coord.GetX());
        EXPECT_FLOAT_EQ(212271.7101, coord.GetY());
        EXPECT_FLOAT_EQ(51.2165845, coord.GetLatitude());
        EXPECT_FLOAT_EQ(4.413545489, coord.GetLongitude());
        EXPECT_EQ("ANTWERPEN", antwerp.GetName());
}

TEST(GeoGridCtorTest, DefaultConstructor)
{
        GeoGrid grid;
        EXPECT_EQ(grid.GetCities().size(), 0);
        EXPECT_EQ(grid.GetTotalPop(), 0);
        EXPECT_EQ(grid.GetTotalPopOfModel(), 0);
        for( auto frac : FractionList )
                EXPECT_FLOAT_EQ(grid.GetFraction(frac), 0.0);
        for( auto size : SizeList )
                EXPECT_EQ(grid.GetAvgSize(size), 0);
        ASSERT_TRUE(grid.GetPopulation() == nullptr);
        EXPECT_EQ(grid.GetInitialSearchRadius(), 0);
        EXPECT_EQ(grid.UsingRandomAges(), false);
        EXPECT_EQ(grid.GetSchoolCount(), 0);
        EXPECT_EQ(grid.IsInitialized(), false);
}

TEST(GeoGridCtorTest, NonExistingFile)
{
        ::testing::FLAGS_gtest_death_test_style = "threadsafe";
        // Test with a non-existing file
        boost::property_tree::ptree p_tree;
        boost::property_tree::read_xml("config/geogen_default.xml", p_tree);
        GeoGrid grid;
        ASSERT_NO_FATAL_FAILURE(grid.Initialize("bad input")); //should now construct a default geogrid...
        EXPECT_EQ(grid.GetCities().size(), 327); //this test is hard to make not hard-coded...
        EXPECT_EQ(grid.GetTotalPop(), p_tree.get<unsigned int>("pop_info.pop_total"));
        EXPECT_EQ(grid.GetTotalPopOfModel(), 4341923); //this test is hard to make not hard-coded...
        EXPECT_FLOAT_EQ(grid.GetFraction(Fractions::ACTIVE), p_tree.get<double>("pop_info.fraction_active_workers"));
        EXPECT_FLOAT_EQ(grid.GetFraction(Fractions::STUDENTS), p_tree.get<double>("pop_info.fraction_students"));
        EXPECT_FLOAT_EQ(grid.GetFraction(Fractions::COMMUTING_STUDENTS), p_tree.get<double>("pop_info.fraction_commuting_students"));
        EXPECT_FLOAT_EQ(grid.GetFraction(Fractions::COMMUTING_WORKERS), p_tree.get<double>("pop_info.fraction_commuting_workers"));
        EXPECT_EQ(grid.GetAvgSize(Sizes::AVERAGE_CP), p_tree.get<unsigned int>("contactpool_info.average_size"));
        EXPECT_EQ(grid.GetAvgSize(Sizes::WORKPLACES), p_tree.get<unsigned int>("contactpool_info.workplace.size"));
        EXPECT_EQ(grid.GetAvgSize(Sizes::SCHOOLS), p_tree.get<unsigned int>("contactpool_info.school.size"));
        EXPECT_EQ(grid.GetAvgSize(Sizes::COMMUNITIES), p_tree.get<unsigned int>("contactpool_info.community.size"));
        EXPECT_EQ(grid.GetAvgSize(Sizes::COLLEGES), p_tree.get<unsigned int>("contactpool_info.college.size"));
        EXPECT_EQ(grid.GetAvgSize(Sizes::MAXLC), p_tree.get<unsigned int>("contactpool_info.college.cities"));
        EXPECT_EQ(grid.GetPopulation()->size(), 0);
        EXPECT_EQ(grid.GetInitialSearchRadius(), p_tree.get<unsigned int>("neighbour_classification.initial_search_radius"));
        EXPECT_EQ(grid.UsingRandomAges(), p_tree.get<bool>("pop_info.random_ages"));
        EXPECT_EQ(grid.GetSchoolCount(), 0);
        EXPECT_EQ(grid.IsInitialized(), true);
}

TEST(GeoGridCtorTest, BadFractions)
{
        ::testing::FLAGS_gtest_death_test_style = "threadsafe";
        GeoGrid grid;
        boost::property_tree::ptree p_tree;
        boost::property_tree::read_xml("config/run_default.xml", p_tree);
        p_tree.put("run.contact_output_file", false);
        p_tree.put("run.rng_type", "lcg64");
        p_tree.put("run.geopop_file", "bad_student_frac_0.xml");
        ASSERT_DEATH_IF_SUPPORTED(grid.Initialize(p_tree), "");
        //ASSERT_NO_FATAL_FAILURE(grid.Reset());
        p_tree.put("run.geopop_file", "bad_stucom_frac_0.xml");
        ASSERT_DEATH_IF_SUPPORTED(grid.Initialize(p_tree), "");
        //ASSERT_NO_FATAL_FAILURE(grid.Reset());
        p_tree.put("run.geopop_file", "bad_active_frac_0.xml");
        ASSERT_DEATH_IF_SUPPORTED(grid.Initialize(p_tree), "");
        //ASSERT_NO_FATAL_FAILURE(grid.Reset());
        p_tree.put("run.geopop_file", "bad_workcom_frac_0.xml");
        ASSERT_DEATH_IF_SUPPORTED(grid.Initialize(p_tree), "");
        //ASSERT_NO_FATAL_FAILURE(grid.Reset());
        p_tree.put("run.geopop_file", "bad_student_frac_1.xml");
        ASSERT_DEATH_IF_SUPPORTED(grid.Initialize(p_tree), "");
        //ASSERT_NO_FATAL_FAILURE(grid.Reset());
        p_tree.put("run.geopop_file", "bad_stucom_frac_1.xml");
        ASSERT_DEATH_IF_SUPPORTED(grid.Initialize(p_tree), "");
        //ASSERT_NO_FATAL_FAILURE(grid.Reset());
        p_tree.put("run.geopop_file", "bad_active_frac_1.xml");
        ASSERT_DEATH_IF_SUPPORTED(grid.Initialize(p_tree), "");
        //ASSERT_NO_FATAL_FAILURE(grid.Reset());
        p_tree.put("run.geopop_file", "bad_workcom_frac_1.xml");
        ASSERT_DEATH_IF_SUPPORTED(grid.Initialize(p_tree), "");
        //ASSERT_NO_FATAL_FAILURE(grid.Reset());

        // This last test will check a contactpool's size that exceeds the cap op 1000...
        p_tree.put("run.geopop_file", "bad_contactpool_size.xml");
        ASSERT_DEATH_IF_SUPPORTED(grid.Initialize(p_tree), "");
}

TEST(GeoGridCtorTest, FaultyCityRow)
{

        // -----------------------------------------------------------------------------------------
        // Parse cities.
        // -----------------------------------------------------------------------------------------
        cout << "Parsing cities." << endl;
        map<unsigned int, City> cty_map;
        unsigned int            totpop = 0;
        bgi::rtree<rtElem, bgi::quadratic<16>> rtree;
        parser::ParseCities("data/flanders_cities_faulty_row.csv", cty_map, totpop, rtree);
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
        bgi::rtree<rtElem, bgi::quadratic<16>> rtree;
        parser::ParseCities("data/flanders_cities_faulty_col.csv", cty_map, totpop, rtree);
        cout << "Done parsing cities." << endl;

        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        EXPECT_EQ(327, cty_map.size());

        City&      antwerp = cty_map.at(11002);
        Coordinate coord   = antwerp.GetCoordinates();

        EXPECT_EQ(1, antwerp.GetProvince());
        EXPECT_EQ(269954, antwerp.GetPopulation());

        EXPECT_FLOAT_EQ(153104.586, coord.GetX());
        EXPECT_FLOAT_EQ(212271.7101, coord.GetY());
        EXPECT_FLOAT_EQ(51.2165845, coord.GetLatitude());
        EXPECT_FLOAT_EQ(4.413545489, coord.GetLongitude());
        EXPECT_EQ("ANTWERPEN", antwerp.GetName());
}

TEST(GeoGridCtorTest, CityRowCounter)
{

        // -----------------------------------------------------------------------------------------
        // Initialize the GeoGrid.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid." << endl;
        GeoGrid grid;
        grid.Initialize("run_default.xml");
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
