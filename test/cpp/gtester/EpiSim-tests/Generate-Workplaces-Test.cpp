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

    class WorkplaceTest : public ::testing::TestWithParam<unsigned int>
    {
    public:
        /// TestCase set up.
        static void SetUpTestCase() {}

        /// Tearing down TestCase
        static void TearDownTestCase() {}

    protected:
        /// Destructor has to be virtual.
        ~WorkplaceTest() override {}

        /// Set up for the test fixture
        void SetUp() override {}

        /// Tearing down the test fixture
        void TearDown() override {}
    private:


    };

    TEST_P(WorkplaceTest, HappyDayScenario)
    {
        // -----------------------------------------------------------------------------------------
        // Initialize the GeoGrid.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid." << endl;
        auto grid = GeoGrid("config/geogen_default.xml");
        cout << "Done building the GeoGrid." << endl;


        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        //Testing 10 randomly chosen cities atm instead of testing all the 327 cities
        unsigned int antwerpen = 3089;
        unsigned int leuven = 877;
        unsigned int brugge = 961;
        unsigned int roeselare = 471;
        unsigned int aalst = 663;
        unsigned int dendermonde = 371;
        unsigned int temse = 213;
        unsigned int sinttruiden = 294;
        unsigned int tongeren = 210;
        unsigned int maasmechelen = 216;


        grid.generate_workplaces();

        auto cities = grid.get_cities();
        EXPECT_EQ(cities[11002]->getNumberOfWorkplaces(), antwerpen);
        EXPECT_EQ(cities[24062]->getNumberOfWorkplaces(), leuven);
        EXPECT_EQ(cities[31005]->getNumberOfWorkplaces(), brugge);
        EXPECT_EQ(cities[36015]->getNumberOfWorkplaces(), roeselare);
        EXPECT_EQ(cities[41002]->getNumberOfWorkplaces(), aalst);
        EXPECT_EQ(cities[42006]->getNumberOfWorkplaces(), dendermonde);
        EXPECT_EQ(cities[46025]->getNumberOfWorkplaces(), temse);
        EXPECT_EQ(cities[71053]->getNumberOfWorkplaces(), sinttruiden);
        EXPECT_EQ(cities[73083]->getNumberOfWorkplaces(), tongeren);
        EXPECT_EQ(cities[73107]->getNumberOfWorkplaces(), maasmechelen);

    }

    TEST_P(WorkplaceTest, CommuterVsLocal)
    {
        // -----------------------------------------------------------------------------------------
        // Initialize the GeoGrid.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid." << endl;
        auto grid = GeoGrid("config/geogen_default.xml");
        cout << "Done building the GeoGrid." << endl;


        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        auto cities = grid.get_cities();
        unsigned int random_city1 = 23096;
        unsigned int random_city2 = 36008;

        //setting local_commuters and no commuters from other cities
        for(auto city: cities){
            cities[random_city1]->SetInCommuters(city.first, 0);
            cities[random_city2]->SetInCommuters(city.first, 0);
        }

        cities[random_city1]->SetInCommuters(random_city1, 200);

        //the other city will have commuters from other cities but no locals
        cities[random_city2]->SetInCommuters(35014, 100);
        cities[random_city2]->SetInCommuters(35029, 50);
        cities[random_city2]->SetInCommuters(37002, 50);

        grid.generate_workplaces();

        EXPECT_EQ(cities[random_city1]->getNumberOfWorkplaces(), cities[random_city2]->getNumberOfWorkplaces());

    }

    TEST_P(WorkplaceTest, Extremeregion)
    {
        // -----------------------------------------------------------------------------------------
        // Initialize the GeoGrid.
        // -----------------------------------------------------------------------------------------
        cout << "Building the GeoGrid." << endl;
        auto grid = GeoGrid("config/geogen_default.xml");
        cout << "Done building the GeoGrid." << endl;


        // -----------------------------------------------------------------------------------------
        // Check results against expected results.
        // -----------------------------------------------------------------------------------------

        auto cities = grid.get_cities();
        unsigned a_random_city = 11004;

        for(auto city: cities){
            cities[a_random_city]->SetInCommuters(city.first, 0);
        }

        grid.generate_workplaces();

        //This is an extrem city where everybody commutes to other cities to work
        EXPECT_EQ(0, cities[a_random_city]->getNumberOfWorkplaces());

    }
    namespace {
//OpenMP should have no effect atm...
#ifdef _OPENMP
        unsigned int threads[]{1U, 4U};
#else
        unsigned int threads[]{1U};
#endif

    } // namespace

    INSTANTIATE_TEST_CASE_P(Run, WorkplaceTest, ValuesIn(threads));

} // namespace Tests


