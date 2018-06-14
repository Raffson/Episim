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

#include "popgen-Episim/generator/GeoGridGenerator.h"
#include "popgen-Episim/generator/PopulationGenerator.h"

namespace Tests {

    using namespace std;
    using namespace ::testing;
    using namespace stride;

    class GenPopTest : public ::testing::TestWithParam<unsigned int> {
    public:
        /// TestCase set up.
        static void SetUpTestCase() {}

        /// Tearing down TestCase
        static void TearDownTestCase() {}

    protected:
        shared_ptr<GeoGrid> grid;


        /// Destructor has to be virtual.
        ~GenPopTest() override {}

        /// Set up for the test fixture
        void SetUp() override {
            grid = GeoGridGenerator().Generate("run_default.xml");
            PopulationGenerator(*grid).Generate();
        }

        /// Tearing down the test fixture
        void TearDown() override {}
    };

    TEST_P(GenPopTest, CityPopulationTest) {
        double margin = 0.01;
        unsigned int pop_counter = 0;
        for (auto &it: grid->GetCities()) {
            City *a_city = &it.second;
            double target = ((double) a_city->GetPopulation() / (double) grid->GetTotalPopOfModel());
            double actual = ((double) a_city->GetEffectivePopulation() / (double) grid->GetTotalPop());
            pop_counter += a_city->GetEffectivePopulation();
            EXPECT_NEAR(actual, target, margin);
        }
        EXPECT_EQ(grid->GetPopulation()->size(), pop_counter);
    }


    TEST_P(GenPopTest, AgeDistributionTest) {

        unsigned int schooled = 0;
        unsigned int youngsters = 0;
        unsigned int middleAged = 0;
        unsigned int toddlers = 0;
        unsigned int oldies = 0;
        unsigned int active = 0;

        unsigned int popCount = grid->GetPopulation()->size();

        for (unsigned int i = 0; i < grid->GetPopulation()->size(); i++) {
            auto aPerson = grid->GetPopulation()->at(i);

            switch (get_category(aPerson.GetAge())) {
                case Fractions::SCHOOLED : {
                    schooled++;
                    break;
                }
                case Fractions::YOUNG : {
                    youngsters++;
                    if (aPerson.GetPoolId(ContactPoolType::Id::Work) != 0) {
                        active++;
                    }
                    break;
                }
                case Fractions::MIDDLE_AGED : {
                    middleAged++;
                    if (aPerson.GetPoolId(ContactPoolType::Id::Work) != 0) {
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

        EXPECT_NEAR((double) schooled / popCount, grid->GetFraction(Fractions::SCHOOLED),
                    margin * grid->GetFraction(Fractions::SCHOOLED));
        EXPECT_NEAR((double) youngsters / popCount, grid->GetFraction(Fractions::YOUNG),
                    margin * grid->GetFraction(Fractions::YOUNG));
        EXPECT_NEAR((double) middleAged / popCount, grid->GetFraction(Fractions::MIDDLE_AGED),
                    margin * grid->GetFraction(Fractions::MIDDLE_AGED));
        EXPECT_NEAR((double) oldies / popCount, grid->GetFraction(Fractions::OLDIES),
                    margin * grid->GetFraction(Fractions::OLDIES));

        double actualActive = (double) active / (middleAged + youngsters);
        //we have approx. 46% active from the people of the working age 4% less than we are supposed to have
        margin = 0.1;
        EXPECT_NEAR(actualActive, grid->GetFraction(Fractions::ACTIVE), margin * grid->GetFraction(Fractions::ACTIVE));

    }


    TEST_P(GenPopTest, CommutersTest) {

        unsigned int studentCommuters = 0;
        unsigned int workerCommuters = 0;
        unsigned int active = 0;
        unsigned int students = 0;

        for(auto it:grid->GetCities()){
            City* aCity = &it.second;
            studentCommuters += aCity->GetStudentCommutersCount();
            workerCommuters += aCity->GetWorkerCommutersCount();
        }

        for (unsigned int i = 0; i < grid->GetPopulation()->size(); i++) {
            auto aPerson = grid->GetPopulation()->at(i);

            if (aPerson.GetPoolId(ContactPoolType::Id::Work) != 0) {
                active ++;
            }

            else if(get_category(aPerson.GetAge()) == Fractions::YOUNG &&
                    aPerson.GetPoolId(ContactPoolType::Id::School) != 0){
                students ++;
            }
        }

        EXPECT_NEAR((double) studentCommuters/students, grid->GetFraction(Fractions::COMMUTING_STUDENTS), 0.05);
        EXPECT_NEAR((double) workerCommuters/active, grid->GetFraction(Fractions::COMMUTING_WORKERS), 0.05);
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