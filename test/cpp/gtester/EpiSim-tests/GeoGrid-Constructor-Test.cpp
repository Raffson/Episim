/**
 * @file
 * Implementation of tests to check the generation of communities.
 */

#include "popgen-Episim/generator/GeoGridGenerator.h"

#include <boost/property_tree/xml_parser.hpp>

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
        // -----------------------------------------------------------------------------------------
        // Initialize the GeoGrid.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid." << endl;
        shared_ptr<GeoGrid> grid;
        ASSERT_NO_FATAL_FAILURE(grid = GeoGridGenerator().Generate("run_default.xml"));
        cout << "Done building the GeoGrid." << endl;

        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        const map<unsigned int, City>& mp = grid->GetCities();

        const City& antwerp = mp.at(11002);
        Coordinate  coord   = antwerp.GetCoordinates();

        EXPECT_EQ(1, antwerp.GetProvince());
        EXPECT_EQ(269954, antwerp.GetPopulation());
        EXPECT_FLOAT_EQ(153104.586, coord.GetX());
        EXPECT_FLOAT_EQ(212271.7101, coord.GetY());
        EXPECT_FLOAT_EQ(51.2165845, coord.GetLatitude());
        EXPECT_FLOAT_EQ(4.413545489, coord.GetLongitude());
        EXPECT_EQ("ANTWERPEN", antwerp.GetName());
}

TEST(GeoGridCtorTest, NonExistingFile)
{
        ::testing::FLAGS_gtest_death_test_style = "threadsafe";
        // Test with a non-existing file
        boost::property_tree::ptree pt;
        boost::property_tree::read_xml("config/geogen_default.xml", pt);
        shared_ptr<GeoGrid> grid;
        ASSERT_NO_FATAL_FAILURE(grid = GeoGridGenerator().Generate("bad input")); //should now construct a default geogrid...
        EXPECT_EQ(grid->GetCities().size(), 327); //this test is hard to make not hard-coded...
        EXPECT_EQ(grid->GetTotalPop(), pt.get<unsigned int>("pop_info.pop_total"));
        EXPECT_EQ(grid->GetTotalPopOfModel(), 4341923); //this test is hard to make not hard-coded...
        EXPECT_FLOAT_EQ(grid->GetFraction(Fractions::ACTIVE), pt.get<double>("pop_info.fraction_active_workers"));
        EXPECT_FLOAT_EQ(grid->GetFraction(Fractions::STUDENTS), pt.get<double>("pop_info.fraction_students"));
        EXPECT_FLOAT_EQ(grid->GetFraction(Fractions::COMMUTING_STUDENTS), pt.get<double>("pop_info.fraction_commuting_students"));
        EXPECT_FLOAT_EQ(grid->GetFraction(Fractions::COMMUTING_WORKERS), pt.get<double>("pop_info.fraction_commuting_workers"));
        EXPECT_EQ(grid->GetAvgSize(Sizes::AVERAGE_CP), pt.get<unsigned int>("contactpool_info.average_size"));
        EXPECT_EQ(grid->GetAvgSize(Sizes::WORKPLACES), pt.get<unsigned int>("contactpool_info.workplace.size"));
        EXPECT_EQ(grid->GetAvgSize(Sizes::SCHOOLS), pt.get<unsigned int>("contactpool_info.school.size"));
        EXPECT_EQ(grid->GetAvgSize(Sizes::COMMUNITIES), pt.get<unsigned int>("contactpool_info.community.size"));
        EXPECT_EQ(grid->GetAvgSize(Sizes::COLLEGES), pt.get<unsigned int>("contactpool_info.college.size"));
        EXPECT_EQ(grid->GetAvgSize(Sizes::MAXLC), pt.get<unsigned int>("contactpool_info.college.cities"));
        EXPECT_EQ(grid->GetPopulation()->size(), 0);
        EXPECT_EQ(grid->GetInitialSearchRadius(), pt.get<unsigned int>("neighbour_classification.initial_search_radius"));
        EXPECT_EQ(grid->UsingRandomAges(), pt.get<bool>("pop_info.random_ages"));
        const double amountSchooled = grid->GetTotalPop() * grid->GetFraction(Fractions::SCHOOLED);
        auto amountOfSchools = (const unsigned int)ceil(amountSchooled / grid->GetAvgSize(Sizes::SCHOOLS));
        EXPECT_EQ(grid->GetSchoolCount(), amountOfSchools);
}

TEST(GeoGridCtorTest, BadFractions)
{
        ::testing::FLAGS_gtest_death_test_style = "threadsafe";
        boost::property_tree::ptree pt;
        boost::property_tree::read_xml("config/run_default.xml", pt);
        pt.put("run.contact_output_file", false);
        array<string, 9> files = {{"bad_student_frac_0.xml", "bad_stucom_frac_0.xml", "bad_active_frac_0.xml",
                               "bad_workcom_frac_0.xml", "bad_student_frac_1.xml", "bad_stucom_frac_1.xml",
                               "bad_active_frac_1.xml", "bad_workcom_frac_1.xml", "bad_contactpool_size.xml"}};
        for( const auto& file : files )
        {
                pt.put("run.geopop_file", file);
                ASSERT_DEATH_IF_SUPPORTED(GeoGridGenerator().Generate(pt), "");
        }
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
        shared_ptr<GeoGrid> grid = GeoGridGenerator().Generate("run_default.xml");
        cout << "Done building the GeoGrid." << endl;

        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        EXPECT_EQ(327, grid->GetCities().size());
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
