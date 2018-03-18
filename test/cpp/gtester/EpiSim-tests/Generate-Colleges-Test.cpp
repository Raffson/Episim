//
// Created by beau on 3/15/18.
//

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
        auto grid = GeoGrid("config/geogen_default.xml");
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
         * 21004,2,86458,149789.4784,172284.1909,50.86079391,4.35941953,BRUSSEL/BRUXELLES-BRUSSEL PENTAGOON/BRUXELLES PENTAGONE
         * 44021,4,141210,104032.0404,194183.7006,51.05097848,3.721264108,GENT-GENT CENTRUM
         * 11002,1,269954,153104.586,212271.7101,51.2165845,4.413545489,ANTWERPEN
         *
        */

        //Expected nr of colleges
        unsigned int expMechelen   = round(45736  * grid.getWorkers1Frac() * grid.getStudentFrac() / grid.getCollegeSize());
        unsigned int expAalst      = round(48564  * grid.getWorkers1Frac() * grid.getStudentFrac() / grid.getCollegeSize());
        unsigned int expElsene     = round(52404  * grid.getWorkers1Frac() * grid.getStudentFrac() / grid.getCollegeSize());
        unsigned int expAnderlecht = round(53489  * grid.getWorkers1Frac() * grid.getStudentFrac() / grid.getCollegeSize());
        unsigned int expLeuven     = round(57258  * grid.getWorkers1Frac() * grid.getStudentFrac() / grid.getCollegeSize());
        unsigned int expSchaarbeek = round(67992  * grid.getWorkers1Frac() * grid.getStudentFrac() / grid.getCollegeSize());
        unsigned int expBrugge     = round(72487  * grid.getWorkers1Frac() * grid.getStudentFrac() / grid.getCollegeSize());
        unsigned int expBrussel    = round(86458  * grid.getWorkers1Frac() * grid.getStudentFrac() / grid.getCollegeSize());
        unsigned int expGent       = round(141210 * grid.getWorkers1Frac() * grid.getStudentFrac() / grid.getCollegeSize());
        unsigned int expAntwerpen  = round(269954 * grid.getWorkers1Frac() * grid.getStudentFrac() / grid.getCollegeSize());

        grid.generate_colleges();

        auto cities = grid.get_cities();
        ASSERT_EQ( cities[12025]->getColleges().size(), expMechelen   );
        ASSERT_EQ( cities[41002]->getColleges().size(), expAalst      );
        ASSERT_EQ( cities[21009]->getColleges().size(), expElsene     );
        ASSERT_EQ( cities[21001]->getColleges().size(), expAnderlecht );
        ASSERT_EQ( cities[24062]->getColleges().size(), expLeuven     );
        ASSERT_EQ( cities[21015]->getColleges().size(), expSchaarbeek );
        ASSERT_EQ( cities[31005]->getColleges().size(), expBrugge     );
        ASSERT_EQ( cities[21004]->getColleges().size(), expBrussel    );
        ASSERT_EQ( cities[44021]->getColleges().size(), expGent       );
        ASSERT_EQ( cities[11002]->getColleges().size(), expAntwerpen  );

        //Now remove these 10 cities and check that all other cities have 0 colleges...
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

        for( auto &it : cities ) {
            ASSERT_EQ( it.second->getColleges().size(), 0 );
        }
    }

    TEST_P(CollegeTest, AlternateScenario)
    {
        //No input / wrong file...
        EXPECT_THROW( auto grid = GeoGrid("bad input..."), runtime_error);
        auto grid = GeoGrid();
        EXPECT_EQ(grid.get_cities().size(), 0);
        EXPECT_EQ(grid.getTotalPop(), 0);
        EXPECT_FLOAT_EQ(grid.getSchooledFrac(), 0.0);
        EXPECT_FLOAT_EQ(grid.getWorkers1Frac(), 0.0);
        EXPECT_FLOAT_EQ(grid.getWorkers2Frac(), 0.0);
        EXPECT_FLOAT_EQ(grid.getRestFrac(), 0.0);
        EXPECT_FLOAT_EQ(grid.getStudentFrac(), 0.0);
        EXPECT_FLOAT_EQ(grid.getCommutingStudents(), 0.0);
        EXPECT_FLOAT_EQ(grid.getActiveFrac(), 0.0);
        EXPECT_FLOAT_EQ(grid.getCommutingWorkers(), 0.0);
        EXPECT_EQ(grid.getSchoolSize(), 0);
        EXPECT_EQ(grid.getCollegeSize(), 0);
        EXPECT_EQ(grid.getMaxLC(), 0);
        EXPECT_EQ(grid.getCommunitySize(), 0);
        EXPECT_EQ(grid.getWorkplaceSize(), 0);

        //Should we really test partial input? That's more like a job for the parser isn't it?
        //In fact, this test all together should be part of the constructor's tester imo...
    }


    //Copy of the code since it is a private function and it is used by adjustLargestCities
    unsigned int findSmallest(const vector <shared_ptr<City>> &lc) {
        unsigned int smallest = 0;
        for (unsigned int i = 1; i < lc.size(); i++) {
            if (lc[smallest]->getPopulation() > lc[i]->getPopulation()) smallest = i;
        }
        return smallest;
    }
    //Copy of the code since it is a private function...
    // small adjustment is needed, m_maxlc must be passed on
    // because this function is no longer part of the GeoGrid class,
    // therefore no direct access to m_maxlc...
    void adjustLargestCities(vector <shared_ptr<City>> &lc, const shared_ptr <City> &city, unsigned int m_maxlc) {
        if (lc.size() < m_maxlc) lc.push_back(city);
        else {
            unsigned int citpop = city->getPopulation();
            unsigned int smallest = findSmallest(lc);
            if (citpop > lc[smallest]->getPopulation()) lc[smallest] = city;
        }
    }

    TEST_P(CollegeTest, adjustLargestCitiesUnit)
    {
        //We'll push 3 cities with m_maxlc=3
        // thus the 4th city will replace iff it has a larger population than one of the 3 cities in lc
        vector <shared_ptr<City>> lc;

        shared_ptr<City> c1 = make_shared<City>(1, 1, 10, Coordinate(), "TestCity1");
        adjustLargestCities(lc, c1, 3);
        ASSERT_EQ(lc.size(), 1);

        shared_ptr<City> c2 = make_shared<City>(2, 1, 20, Coordinate(), "TestCity2");
        adjustLargestCities(lc, c2, 3);
        ASSERT_EQ(lc.size(), 2);

        shared_ptr<City> c3 = make_shared<City>(3, 1, 30, Coordinate(), "TestCity3");
        adjustLargestCities(lc, c3, 3);
        ASSERT_EQ(lc.size(), 3);

        //Try adding a city with smaller population than other cities...
        // Size should remain 3 and c1, c2, c3 in lc...
        shared_ptr<City> c4 = make_shared<City>(4, 1, 5, Coordinate(), "TestCity4");
        adjustLargestCities(lc, c4, 3);
        ASSERT_EQ(lc.size(), 3);
        ASSERT_EQ(lc[0], c1);
        ASSERT_EQ(lc[1], c2);
        ASSERT_EQ(lc[2], c3);

        //Try adding a city with larger population than other cities...
        // Size should remain 3 and c5, c2, c3 in lc...
        shared_ptr<City> c5 = make_shared<City>(5, 1, 15, Coordinate(), "TestCity4");
        adjustLargestCities(lc, c5, 3);
        ASSERT_EQ(lc.size(), 3);
        ASSERT_EQ(lc[0], c5);
        ASSERT_EQ(lc[1], c2);
        ASSERT_EQ(lc[2], c3);
    }

    //In the testplan I mentioned "assignCollege"
    // however at this point I figured out that this should be done for City
    // specifically testing "addCommunity", and perhaps the rest of the class...

    namespace {
//OpenMP should have no effect atm...
//Should we simply leave this out?
#ifdef _OPENMP
        unsigned int threads[]{1U, 4U};
#else
        unsigned int threads[]{1U};
#endif

    } // namespace

    INSTANTIATE_TEST_CASE_P(Run, CollegeTest, ValuesIn(threads));

} // namespace Tests


