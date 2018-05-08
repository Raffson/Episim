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

    GenPopTest(){
        grid.Initialize("run_default.xml");
        grid.GenerateAll();

        stride::PopulationGenerator pop_generator(grid);
        pop_generator.GeneratePopulation();
    }

protected:
    stride::GeoGrid grid;

    /// Destructor has to be virtual.
    ~GenPopTest() override {}

    /// Set up for the test fixture
    void SetUp() override {}

    /// Tearing down the test fixture
    void TearDown() override {}
};

TEST_P(GenPopTest, CityPopulationTest)
{
    double margin = 0.11;
    unsigned int pop_counter = 0;
    for(auto& it: grid.GetCities()){
        City* a_city = &it.second;
        double target =  ((double)a_city->GetPopulation() / (double)grid.GetTotalPopOfModel());
        double actual = ((double)a_city->GetEffectivePopulation() / (double)grid.GetTotalPop());
        pop_counter += a_city->GetEffectivePopulation();
        EXPECT_NEAR(actual, target, target * margin);
    }
    EXPECT_EQ(grid.GetPopulation()->size(), pop_counter);
}


TEST_P(GenPopTest, AgeDistributionTest)
{

    unsigned int schooled = 0;
    unsigned int youngsters = 0;
    unsigned int middle_aged = 0;
    unsigned int toddlers = 0;
    unsigned int oldies = 0;
    unsigned int active = 0;

    unsigned int pop_count = grid.GetPopulation()->size();

    for(unsigned int i=0; i<grid.GetPopulation()->size(); i++){
        auto a_person = grid.GetPopulation()->at(i);

        switch(get_category(a_person.GetAge())) {
            case Fractions::SCHOOLED : {
                schooled++;
                break;
            }
            case Fractions::YOUNG : {
                youngsters++;
                if(a_person.GetPoolId(ContactPoolType::Id::Work) != 0){
                    active++;
                }
                break;
            }
            case Fractions::MIDDLE_AGED : {
                middle_aged++;
                if(a_person.GetPoolId(ContactPoolType::Id::Work) != 0){
                    active++;
                }
                break;
            }
            case Fractions::TODDLERS : {
                toddlers++;
                break;
            }
            default: {
                oldies++;
                break;
            }
        }
    }

    double margin = 0.01;

    EXPECT_NEAR((double)schooled/pop_count, grid.GetFraction(Fractions::SCHOOLED),
                margin*grid.GetFraction(Fractions::SCHOOLED));
    EXPECT_NEAR((double)youngsters/pop_count, grid.GetFraction(Fractions::YOUNG),
                margin*grid.GetFraction(Fractions::YOUNG));
    EXPECT_NEAR((double)middle_aged/pop_count, grid.GetFraction(Fractions::MIDDLE_AGED),
                margin*grid.GetFraction(Fractions::MIDDLE_AGED));
    EXPECT_NEAR((double)oldies/pop_count, grid.GetFraction(Fractions::OLDIES),
                margin*grid.GetFraction(Fractions::OLDIES));

    double actual_active = (double) active / (middle_aged + youngsters);
    //we have approx. 46% active from the people of the working age 4% less than we are supposed to have
    margin = 0.1;
    EXPECT_NEAR(actual_active, grid.GetFraction(Fractions::ACTIVE), margin * grid.GetFraction(Fractions::ACTIVE));
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