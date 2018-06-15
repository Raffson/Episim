//
// Created by Nishchal Shrestha on 20/03/2018.
//

#include "PopulationGenerator.h"
#include "pop/Population.h"
#include "pop/SurveySeeder.h"
#include "popgen-Episim/model/Household.h"
#include "popgen-Episim/util/DesignByContract.h"

#include "trng/discrete_dist.hpp"
#include "trng/uniform_dist.hpp"
#include "trng/uniform_int_dist.hpp"

#include <cmath>
#include <ctime>
#include <iterator>

#include <omp.h>

using namespace std;

namespace stride {

PopulationGenerator::PopulationGenerator(GeoGrid& geogrid) :
        m_grid(geogrid), m_rng(geogrid.GetRNG())
{
        InitializeHouseholdFractions();
        InitializeCommutingFractions();
        InitializeCityPopFractions();
}

void PopulationGenerator::InitializeHouseholdFractions()
{
        const auto& households = m_grid.GetModelHouseholds();
        for (const auto& elem : households)
                m_household_size_fracs.emplace_back(elem.second.size());
}

void PopulationGenerator::InitializeCommutingFractions()
{
        const double commutingStudentsFrac = m_grid.GetFraction(Fractions::COMMUTING_STUDENTS);
        const double studentFrac            = m_grid.GetFraction(Fractions::STUDENTS);
        const double youngWorkersFrac      = m_grid.GetFraction(Fractions::YOUNG);
        const double studentCommutersFrac  = commutingStudentsFrac * studentFrac * youngWorkersFrac;
        for (const auto& cityA : m_grid.GetCities()) // for each cityA...
        {
                m_city_ids.emplace_back(cityA.first);
                if (cityA.second.HasCommunityType(CommunityType::Id::College))
                        m_college_ids.emplace_back(cityA.first);
                vector<double> workerDist;
                vector<double> studentDist;
                double         commutersA = cityA.second.GetTotalOutCommutersCount();
                for (const auto& cityB : m_grid.GetCities()) // calculate the chance to commute from cityA to cityB
                {
                        const bool hasCollege = cityB.second.HasCommunityType(CommunityType::Id::College);
                        // We don't want local commuting
                        if (cityA.first != cityB.first) {
                                if (hasCollege) {
                                        // Calculate the nr of commuting students
                                        double students = cityA.second.GetPopulation() * studentCommutersFrac;
                                        students *= (cityA.second.GetOutCommuting().at(cityB.first) / commutersA);
                                        workerDist.emplace_back(cityA.second.GetOutCommuting().at(cityB.first) -
                                                                 students);
                                        studentDist.emplace_back(students);
                                } else
                                        workerDist.emplace_back(cityA.second.GetOutCommuting().at(cityB.first));

                        } else // just push a 0, this will make sure this particular index can't be chosen...
                        {
                                workerDist.emplace_back(0);
                                if (hasCollege)
                                        studentDist.emplace_back(0);
                        }
                }
                check_distribution(workerDist);
                check_distribution(studentDist);
                m_worker_commuting_fracs[cityA.first]  = workerDist;
                m_student_commuting_fracs[cityA.first] = studentDist;
        }
}

void PopulationGenerator::InitializeCityPopFractions()
{
        double totalpop = m_grid.GetTotalPop();
        for (const auto& city : m_grid.GetCities())
                m_city_pop_fracs.emplace_back(city.second.GetPopulation() / totalpop);
}

unsigned int PopulationGenerator::GetRandomHouseholdSize()
{
        trng::discrete_dist distr(m_household_size_fracs.begin(), m_household_size_fracs.end());
        // plus 1 because discrete_dist returns numbers between 0 and (m_household_size_fracs.size() - 1)
        // we need numbers between 1 and m_household_size_fracs.size()
        return (unsigned int)(m_rng.GetGenerator(distr,omp_get_thread_num())() + 1);
}

const vector<double>& PopulationGenerator::GetRandomModelHouseholdOfSize(unsigned int size)
{
    const auto& households = m_grid.GetModelHouseholds().at(size);
    trng::uniform_int_dist distr(0, households.size());
    return households[m_rng.GetGenerator(distr,omp_get_thread_num())()];
}

double PopulationGenerator::GetRandomAge(Fractions category)
{
        switch (category) {
        case Fractions::SCHOOLED : // [3, 18)
                return (double)m_rng.GetGenerator(trng::uniform_dist<double>(3.0, 18.0),omp_get_thread_num())();
        case Fractions::YOUNG : // [18, 26)
                return (double)m_rng.GetGenerator(trng::uniform_dist<double>(18.0, 26.0),omp_get_thread_num())();
        case Fractions::MIDDLE_AGED : // [26, 65)
                return (double)m_rng.GetGenerator(trng::uniform_dist<double>(26.0, 65.0),omp_get_thread_num())();
        case Fractions::TODDLERS : // [0, 3)
                return (double)m_rng.GetGenerator(trng::uniform_dist<double>(0.0, 3.0),omp_get_thread_num())();
        case Fractions::OLDIES : // [65, 81), cause maximum age according to Age.h is 80...
                // gotta improve this since we would need [65, 80] but not with a uniform distribution...
                // because the chances you become older get smaller and smaller right?
                return (double)m_rng.GetGenerator(trng::uniform_dist<double>(65.0, 81.0),omp_get_thread_num())();
        default:
                // throw an exception here instead?
                return (double)m_rng.GetGenerator(trng::uniform_dist<double>(0.0, 81.0),omp_get_thread_num())();
        }
}

City& PopulationGenerator::GetRandomCity()
{
        trng::discrete_dist distr(m_city_pop_fracs.begin(), m_city_pop_fracs.end());
        return m_grid[m_city_ids[(unsigned int)m_rng.GetGenerator(distr, omp_get_thread_num())()]];
}

ContactPool* PopulationGenerator::GetRandomContactPool(const vector<Community*>& comms)
{
        if (comms.empty())
                return nullptr;

        trng::uniform_int_dist distr(0, comms.size());
        unsigned int           index = (unsigned int)m_rng.GetGenerator(distr, omp_get_thread_num())();
        auto&                  pools = comms.at(index)->GetContactPools();
        if (!pools.empty()) {
                trng::uniform_int_dist pdistr(0, pools.size());
                unsigned int           index2 = (unsigned int)m_rng.GetGenerator(pdistr, omp_get_thread_num())();
                return pools[index2];
        } else
                return nullptr;
}

// Unfair, unless you pass frac=0.5
const bool PopulationGenerator::FlipUnfairCoin(const double& frac)
{
        vector<double> fracs;
        fracs.emplace_back(1 - frac);
        fracs.emplace_back(frac);
        trng::discrete_dist distr(fracs.begin(), fracs.end());
        return (const bool)m_rng.GetGenerator(distr,omp_get_thread_num())();
}

const bool PopulationGenerator::IsWorkingCommuter()
{
        return FlipUnfairCoin(m_grid.GetFraction(Fractions::COMMUTING_WORKERS));
}

const bool PopulationGenerator::IsStudentCommuter()
{
        return FlipUnfairCoin(m_grid.GetFraction(Fractions::COMMUTING_STUDENTS));
}

const bool PopulationGenerator::IsStudent() { return FlipUnfairCoin(m_grid.GetFraction(Fractions::STUDENTS)); }

const bool PopulationGenerator::IsActive() { return FlipUnfairCoin(m_grid.GetFraction(Fractions::ACTIVE)); }

ContactPool* PopulationGenerator::AssignWorkerAtRandom(City& origin)
{
        if (IsWorkingCommuter()) {
                City& city = GetRandomCommutingCity(origin);
                return GetRandomContactPool(city.GetCommunitiesOfType(CommunityType::Id::Work));
        } else
                return GetRandomContactPool(GetRandomCommunities(origin, CommunityType::Id::Work));
}

Person* PopulationGenerator::GeneratePerson(const double& age, const size_t& hid, const size_t& scid,
                                         Population& pop, City& city)
{
        Fractions    category  = get_category(age);
        ContactPool* school    = nullptr;
        ContactPool* workplace = nullptr;
        ContactPool* primcomm  = GetRandomContactPool(GetRandomCommunities(city, CommunityType::Id::Primary));
        if (category == Fractions::SCHOOLED) { // [3, 18)
                school = GetRandomContactPool(GetRandomCommunities(city, CommunityType::Id::School));
        } else if (category == Fractions::YOUNG) { // [18, 26)
                // first check if we have a student or not...
                if (IsStudent()) {
                        if (IsStudentCommuter()) {
                                City& c = GetRandomCommutingCity(city, true);
                                school = GetRandomContactPool(c.GetCommunitiesOfType(CommunityType::Id::College));
                        } else
                                school = GetRandomContactPool(GetNearestColleges(city));
                } else if (IsActive())
                        workplace = AssignWorkerAtRandom(city);
        } else if (category == Fractions::MIDDLE_AGED and IsActive()) { // [26, 65)
                workplace = AssignWorkerAtRandom(city);
        }
        size_t schoolid = (school) ? school->GetID() : 0;
        size_t workid   = (workplace) ? workplace->GetID() : 0;
        size_t pcid     = (primcomm) ? primcomm->GetID() : 0;

        Person *person;

#pragma omp critical(pop)
    {
        person = pop.emplace_back(pop.size(), age, hid, schoolid, workid, pcid, scid);
    }

    // Add the person to the contactpools, if any...
#pragma critical(contact_pool)
    {
        if (school)
            school->AddMember(person);

        if (workplace)
            workplace->AddMember(person);

        if (primcomm)
            primcomm->AddMember(person);
    }

    return person;
}

void PopulationGenerator::GenerateHousehold(unsigned int size, City& city)
{
        auto&        pop           = *m_grid.GetPopulation();

        auto&        pool_sys      = pop.GetContactPoolSys();
        Household* theHousehold;

#pragma omp critical(add_household)
        {
                theHousehold = &city.AddHousehold(pool_sys);
        }
        size_t       hid           = theHousehold->GetID();

        ContactPool *seccomm  = GetRandomContactPool(GetRandomCommunities(city, CommunityType::Id::Secondary));

        // Meaning you always get assigned to a community?
        size_t scid = (seccomm) ? seccomm->GetID() : 0;

        for (auto age : GetRandomModelHouseholdOfSize(size))
        {
                if( m_grid.UsingRandomAges() ) {
                    Fractions category = get_category(age);
                    age = GetRandomAge(category);
                }
                //if( model_household.size() == 1 and category == Fractions::SCHOOLED)
                //    cout << "Lonely minor of model age=" << model_household[i] << " has now received age=" << age << endl;


                Person* p = GeneratePerson(age, hid, scid, pop, city);

                theHousehold->AddMember(p);

                if (seccomm) {

                    seccomm->AddMember(p);
                }

        }
}


void PopulationGenerator::Generate()
{
        // TODO: currently it takes about 20sec to generate 4.3 million people,
        // TODO: this should be improved even more if possible...

        cout << "Starting population generation..." << endl;
        double beginTime     = omp_get_wtime();
        long long int remainingPop  = m_grid.GetTotalPop(); // long long to make sure the unsigned int fits...
        long int threadedPop = remainingPop / omp_get_max_threads();

#pragma omp parallel for schedule(static)
        for(int i = 0; i < omp_get_max_threads(); i++) {
                long int localThreadedPop = threadedPop;
                while (localThreadedPop > 0) {
                        City &city = GetRandomCity();
                        auto householdSize = GetRandomHouseholdSize();
                        if (localThreadedPop - householdSize < 0)
                                householdSize = (unsigned int) ceil(localThreadedPop);

                        GenerateHousehold(householdSize, city);
                        localThreadedPop -= householdSize;
                }

        }

        //Fixing rest of dividing by thread
        remainingPop = remainingPop % omp_get_max_threads();
        while(remainingPop > 0){
            City &city = GetRandomCity();
            auto householdSize = GetRandomHouseholdSize();
            if (remainingPop - householdSize < 0)
                householdSize = (unsigned int) remainingPop;

            GenerateHousehold(householdSize, city);
            remainingPop -= householdSize;
        }

        cout << "Done generating population, time needed = " << omp_get_wtime() - beginTime << endl;
        SurveySeeder(m_grid.GetConfigPtree(), m_rng).Seed(m_grid.GetPopulation());
}

const vector<Community*>& PopulationGenerator::GetRandomCommunities(const City& city, const CommunityType::Id& type)
{
        unsigned int radius = m_grid.GetInitialSearchRadius();
        REQUIRE(radius > 0, "Initial search radius of the GeoGrid must be bigger than 0.");
        while (radius != 0) {

                const vector<City *> &cities = m_grid.GetCitiesWithinRadiusWithCommunityType(city, radius, type);

                if (!cities.empty()) {
                        auto index = (unsigned int)m_rng.GetGenerator(trng::uniform_int_dist(0, cities.size()),
                                omp_get_thread_num())();
                        return cities[index]->GetCommunitiesOfType(type);
                }
                radius <<= 1; // equivalent to multiplying by 2
        }
        // throw exception here instead?
        return const_cast<City&>(city).GetCommunitiesOfType(type);
}

std::vector<Community*> PopulationGenerator::GetNearestColleges(const City& origin)
{
        vector<Community*>   result;
        const vector<City*>& cities(m_grid.GetCitiesWithCollege());
        double               nearest = numeric_limits<double>::max();
        unsigned int         nindex  = 0;
        for (unsigned int i = 0; i < cities.size(); i++) {
                double distance = cities[i]->GetCoordinates().GetDistance(origin.GetCoordinates());
                if (distance < nearest) {
                        nearest = distance;
                        nindex  = i;
                }
        }
        for (auto college : cities[nindex]->GetCommunitiesOfType(CommunityType::Id::College))
                result.emplace_back(college);
        return result;
}

City& PopulationGenerator::GetRandomCommutingCity(City& origin, const bool student)
{
        const vector<double>& distribution = student ? m_student_commuting_fracs[origin.GetId()]
                                                     : m_worker_commuting_fracs[origin.GetId()];
        trng::discrete_dist distr(distribution.begin(), distribution.end());
        auto                index = (const unsigned int)m_rng.GetGenerator(distr,omp_get_thread_num())();

        const unsigned int  id    = student ? m_college_ids.at(index) : m_city_ids.at(index);
        origin.AddEffectiveCommuterTo(id);
        return m_grid.GetCities().at(id);

}

} // namespace stride
