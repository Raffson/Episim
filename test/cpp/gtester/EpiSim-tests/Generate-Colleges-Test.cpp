//
// Created by beau on 3/15/18.
//

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

class CollegeTest : public ::testing::TestWithParam<unsigned int>
{
public:
        /// TestCase set up.
        static void SetUpTestCase() {}

        /// Tearing down TestCase
        static void TearDownTestCase() {}

protected:
        /// Destructor has to be virtual.
        ~CollegeTest() override {}

        /// Set up for the test fixture
        void SetUp() override {}

        /// Tearing down the test fixture
        void TearDown() override {}
};

TEST_P(CollegeTest, HappyDayScenario)
{

        // -----------------------------------------------------------------------------------------
        // Initialise the GeoGrid.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid." << endl;
        GeoGrid grid;
        grid.Initialize("config/geogen_default.xml");
        cout << "Done building the GeoGrid." << endl;

        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------
        /* 10 largest cities of flanders_cities_test.csv
         * id,province,population,x_coord,y_coord,latitude,longitude,name
         * 12025,1,45736,157133.0047,190719.1868,51.02534575,4.474594428,MECHELEN
         * 41002,4,48564,128207.1771,181656.3761,50.94284754,4.057880128,AALST
         * 21009,2,52404,150462.2998,168070.6376,50.8270563,4.372308,ELSENE/IXELLES
         * 21001,2,53489,145296.4584,169229.9655,50.83348931,4.297707164,ANDERLECHT
         * 24062,2,57258,173931.7171,174757.824,50.86698143,4.711928375,LEUVEN
         * 21015,2,67992,151137.8935,172542.4363,50.86744,4.37727,SCHAARBEEK/SCHAERBEEK
         * 31005,3,72487,69778.44607,212973.689,51.21760152,3.220079748,BRUGGE
         * 21004,2,86458,149789.4784,172284.1909,50.86079391,4.35941953,BRUSSEL/BRUXELLES
         * 44021,4,141210,104032.0404,194183.7006,51.05097848,3.721264108,GENT-GENT CENTRUM
         * 11002,1,269954,153104.586,212271.7101,51.2165845,4.413545489,ANTWERPEN
         *
         */

        // Expected nr of colleges
        double       stufrac       = grid.GetFraction(Fractions::STUDENTS);
        double       ywfrac        = grid.GetFraction(Fractions::YOUNG);
        unsigned int colsize       = grid.GetAvgSize(Sizes::COLLEGES);
        unsigned int expMechelen   = round(45736 * ywfrac * stufrac / colsize);
        unsigned int expAalst      = round(48564 * ywfrac * stufrac / colsize);
        unsigned int expElsene     = round(52404 * ywfrac * stufrac / colsize);
        unsigned int expAnderlecht = round(53489 * ywfrac * stufrac / colsize);
        unsigned int expLeuven     = round(57258 * ywfrac * stufrac / colsize);
        unsigned int expSchaarbeek = round(67992 * ywfrac * stufrac / colsize);
        unsigned int expBrugge     = round(72487 * ywfrac * stufrac / colsize);
        unsigned int expBrussel    = round(86458 * ywfrac * stufrac / colsize);
        unsigned int expGent       = round(141210 * ywfrac * stufrac / colsize);
        unsigned int expAntwerpen  = round(269954 * ywfrac * stufrac / colsize);

        grid.GenerateColleges();

        auto cities = grid.GetCities();
        ASSERT_EQ(cities.at(12025).GetColleges().size(), expMechelen);
        ASSERT_EQ(cities.at(41002).GetColleges().size(), expAalst);
        ASSERT_EQ(cities.at(21009).GetColleges().size(), expElsene);
        ASSERT_EQ(cities.at(21001).GetColleges().size(), expAnderlecht);
        ASSERT_EQ(cities.at(24062).GetColleges().size(), expLeuven);
        ASSERT_EQ(cities.at(21015).GetColleges().size(), expSchaarbeek);
        ASSERT_EQ(cities.at(31005).GetColleges().size(), expBrugge);
        ASSERT_EQ(cities.at(21004).GetColleges().size(), expBrussel);
        ASSERT_EQ(cities.at(44021).GetColleges().size(), expGent);
        ASSERT_EQ(cities.at(11002).GetColleges().size(), expAntwerpen);

        // Now remove these 10 cities and check that all other cities have 0 colleges...
        cities.erase(12025);
        cities.erase(41002);
        cities.erase(21009);
        cities.erase(21001);
        cities.erase(24062);
        cities.erase(21015);
        cities.erase(31005);
        cities.erase(21004);
        cities.erase(44021);
        cities.erase(11002);

        for (auto& it : cities) {
                ASSERT_EQ(it.second.GetColleges().size(), 0);
        }
}

TEST_P(CollegeTest, WrongInput)
{
        // -----------------------------------------------------------------------------------------
        // Initialize the GeoGrid.
        // -----------------------------------------------------------------------------------------

        /* After reworking the fractions (deduced from households) these death tests won't die anymore...
         * need some new relevant tests here...
         * currently i'm leaving this in comments, as well as leaving the xml files in place
         * this (code below + files) should be deleted in the future (or reworked)
         *
        // Bad input file with fractions that are FUBAR
        auto grid = GeoGrid("config/schools&colleges_bad_frac_0.xml");
        ASSERT_DEATH_IF_SUPPORTED(grid.GenerateColleges(), "");
        grid = GeoGrid("config/schools&colleges_bad_frac_1.xml");
        ASSERT_DEATH_IF_SUPPORTED(grid.GenerateColleges(), "");
        */

        // uncomment the next 2 lines once the "unsigned int refractor" has happened...
        // grid = GeoGrid("config/bad_community_sizes.xml");
        // ASSERT_DEATH_IF_SUPPORTED(grid.GenerateColleges(), "");
}

// Copy of the code since it is a private function and it is used by AdjustLargestCities
unsigned int findSmallest(const vector<City*>& lc)
{
        unsigned int smallest = 0;
        for (unsigned int i = 1; i < lc.size(); i++) {
                if (lc[smallest]->GetPopulation() > lc[i]->GetPopulation())
                        smallest = i;
        }
        return smallest;
}
// Copy of the code since it is a private function...
// small adjustment is needed, m_maxlc must be passed on
// because this function is no longer part of the GeoGrid class,
// therefore no direct access to m_maxlc...
void adjustLargestCities(vector<City*>& lc, City& city, unsigned int m_maxlc)
{
        if (lc.size() < m_maxlc)
                lc.push_back(&city);
        else {
                unsigned int citpop   = city.GetPopulation();
                unsigned int smallest = findSmallest(lc);
                if (citpop > lc[smallest]->GetPopulation())
                        lc[smallest] = &city;
        }
}

TEST_P(CollegeTest, adjustLargestCitiesUnit)
{
        // We'll push 3 cities with m_maxlc=3
        // thus the 4th city will replace iff it has a larger population than one of the 3 cities in lc
        vector<City*> lc;

        City c1(1, 1, 10, Coordinate(), "TestCity1");
        adjustLargestCities(lc, c1, 3);
        ASSERT_EQ(lc.size(), 1);

        City c2(2, 1, 20, Coordinate(), "TestCity2");
        adjustLargestCities(lc, c2, 3);
        ASSERT_EQ(lc.size(), 2);

        City c3(3, 1, 30, Coordinate(), "TestCity3");
        adjustLargestCities(lc, c3, 3);
        ASSERT_EQ(lc.size(), 3);

        // Try adding a city with smaller population than other cities...
        // Size should remain 3 and c1, c2, c3 in lc...
        City c4(4, 1, 5, Coordinate(), "TestCity4");
        adjustLargestCities(lc, c4, 3);
        ASSERT_EQ(lc.size(), 3);
        ASSERT_EQ(lc[0], &c1); // comparing addresses, if they're the same => same object...
        ASSERT_EQ(lc[1], &c2);
        ASSERT_EQ(lc[2], &c3);

        // Try adding a city with larger population than other cities...
        // Size should remain 3 and c5, c2, c3 in lc...
        City c5(5, 1, 15, Coordinate(), "TestCity4");
        adjustLargestCities(lc, c5, 3);
        ASSERT_EQ(lc.size(), 3);
        ASSERT_EQ(lc[0], &c5);
        ASSERT_EQ(lc[1], &c2);
        ASSERT_EQ(lc[2], &c3);
}

// In the testplan I mentioned "assignCollege"
// however at this point I figured out that this should be done for City
// specifically testing "AddCommunity", and perhaps the rest of the class...

namespace {
// OpenMP should have no effect atm...
// Should we simply leave this out?
// Robbe: Keep the ifdef if we ever want to test on multiple threads. Changed the array
//        to only one value bcz it just duplicates tests at the moment.
#ifdef _OPENMP
unsigned int threads[]{1U};
#else
unsigned int threads[]{1U};
#endif

} // namespace

INSTANTIATE_TEST_CASE_P(Run, CollegeTest, ValuesIn(threads));

} // namespace Tests
