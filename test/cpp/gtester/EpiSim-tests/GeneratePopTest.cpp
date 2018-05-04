//
// Created by Nishchal Shrestha on 30/04/2018.
//

/*
 * @file
 * Test for generation of population
 */
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <gtest/gtest.h>
#include <omp.h>
#include <spdlog/spdlog.h>

#include "popgen-Episim/GeoGen/GeoGrid.h"
#include "popgen-Episim/PopGen/PopulationGenerator.h"

namespace Tests{

using namespace std;
using namespace ::testing;
using namespace stride;

class GenPopTest: public ::testing::TestWithParam<unsigned int>
{
public:
    /// TestCase set up.
    static void SetUpTestCase() {}

    /// Tearing down TestCase
    static void TearDownTestCase() {}

protected:
    /// Destructor has to be virtual.
    ~GenPopTest() override {}

    /// Set up for the test fixture
    void SetUp() override {}

    /// Tearing down the test fixture
    void TearDown() override {}
};

TEST_P(GenPopTest, CityPopulationTest)
{
    stride::GeoGrid grid;
    ASSERT_NO_FATAL_FAILURE(grid.Initialize("run_default.xml"));
    grid.GenerateAll();

    stride::PopulationGenerator pop_generator(grid);
    pop_generator.GeneratePopulation();
    
    double margin = 0.15;

    for(auto& it: grid.GetCities()){
        City* a_city = &it.second;
        double actual =  ((double)a_city->GetPopulation() / (double)grid.GetTotalPopOfModel());
        double target = ((double)a_city->GetEffectivePopulation() / (double)grid.GetTotalPop());
        EXPECT_NEAR(actual, target, actual * margin);
    }

}

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

INSTANTIATE_TEST_CASE_P(Run, GenPopTest, ValuesIn(threads));


}//end namespace Tests