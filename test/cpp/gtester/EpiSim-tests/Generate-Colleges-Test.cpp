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
        // Initialise the simulator.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid. " << endl;
        auto grid = GeoGrid("config/geogen_default.xml");
        cout << "Done building the GeoGrid" << endl;

        // -----------------------------------------------------------------------------------------
        // Check results against target number.
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

    }

    TEST_P(CollegeTest, AlternateScenario)
    {

        // -----------------------------------------------------------------------------------------
        // Initialise the simulator.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid. " << endl;
        auto grid = GeoGrid("config/geogen_default.xml");
        cout << "Done building the GeoGrid" << endl;

        // -----------------------------------------------------------------------------------------
        // Check results against target number.
        // -----------------------------------------------------------------------------------------

        //Do the test...
    }

    TEST_P(CollegeTest, adjustLargestCitiesUnit)
    {

        // -----------------------------------------------------------------------------------------
        // Initialise the simulator.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid. " << endl;
        auto grid = GeoGrid("config/geogen_default.xml");
        cout << "Done building the GeoGrid" << endl;

        // -----------------------------------------------------------------------------------------
        // Check results against target number.
        // -----------------------------------------------------------------------------------------

        //Do the test...
    }

    //In the testplan I mentioned "assignCollege"
    // however at this point I figured out that this should be done for City
    // specifically testing "addCommunity", and perhaps the rest of the class...

    namespace {
//OpenMP should have no effect atm...
#ifdef _OPENMP
        unsigned int threads[]{1U, 4U};
#else
        unsigned int threads[]{1U};
#endif

    } // namespace

    INSTANTIATE_TEST_CASE_P(Run, CollegeTest, ValuesIn(threads));

} // namespace Tests


