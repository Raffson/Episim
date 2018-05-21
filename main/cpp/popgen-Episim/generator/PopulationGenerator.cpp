//
// Created by Nishchal Shrestha on 20/03/2018.
//

#include "PopulationGenerator.h"
#include "pop/SurveySeeder.h"
#include "popgen-Episim/model/Household.h"
#include "popgen-Episim/util/DesignByContract.h"

#include "trng/discrete_dist.hpp"
#include "trng/uniform_dist.hpp"
#include "trng/uniform_int_dist.hpp"

#include <cmath>
#include <ctime>
#include <iterator>

using namespace std;

namespace stride {

unsigned int PopulationGenerator::m_id_generator = 1;

PopulationGenerator::PopulationGenerator(GeoGrid& geogrid) : m_geogrid(geogrid), m_rng(geogrid.GetRNG())
{
        InitializeHouseholdFractions();
        InitializeCommutingFractions();
        InitializeCityPopFractions();
}

void PopulationGenerator::InitializeHouseholdFractions()
{
        const auto& households = m_geogrid.GetModelHouseholds();
        for (const auto& elem : households)
                m_household_size_fracs.emplace_back(elem.second.size());
}

void PopulationGenerator::InitializeCommutingFractions()
{
        const double commuting_students_frac = m_geogrid.GetFraction(Fractions::COMMUTING_STUDENTS);
        const double student_frac            = m_geogrid.GetFraction(Fractions::STUDENTS);
        const double young_workers_frac      = m_geogrid.GetFraction(Fractions::YOUNG);
        const double student_commuters_frac  = commuting_students_frac * student_frac * young_workers_frac;
        for (const auto& cityA : m_geogrid.GetCities()) // for each cityA...
        {
                m_city_ids.emplace_back(cityA.first);
                if (cityA.second.HasCommunityType(CommunityType::Id::College))
                        m_college_ids.emplace_back(cityA.first);
                vector<double> worker_dist;
                vector<double> student_dist;
                double         commutersA = cityA.second.GetTotalOutCommutersCount();
                for (const auto& cityB : m_geogrid.GetCities()) // calculate the chance to commute from cityA to cityB
                {
                        const bool has_college = cityB.second.HasCommunityType(CommunityType::Id::College);
                        // We don't want local commuting
                        if (cityA.first != cityB.first) {
                                if (has_college) {
                                        // Calculate the nr of commuting students
                                        double students = cityA.second.GetPopulation() * student_commuters_frac;
                                        students *= (cityA.second.GetOutCommuting().at(cityB.first) / commutersA);
                                        worker_dist.emplace_back(cityA.second.GetOutCommuting().at(cityB.first) -
                                                                 students);
                                        student_dist.emplace_back(students);
                                } else
                                        worker_dist.emplace_back(cityA.second.GetOutCommuting().at(cityB.first));

                        } else // just push a 0, this will make sure this particular index can't be chosen...
                        {
                                worker_dist.emplace_back(0);
                                if (has_college)
                                        student_dist.emplace_back(0);
                        }
                }
                check_distribution(worker_dist);
                check_distribution(student_dist);
                m_worker_commuting_fracs[cityA.first]  = worker_dist;
                m_student_commuting_fracs[cityA.first] = student_dist;
        }
}

void PopulationGenerator::InitializeCityPopFractions()
{
        double totalpop = m_geogrid.GetTotalPop();
        for (const auto& city : m_geogrid.GetCities())
                m_city_pop_fracs.emplace_back(city.second.GetPopulation() / totalpop);
}

unsigned int PopulationGenerator::GetRandomHouseholdSize()
{
        trng::discrete_dist distr(m_household_size_fracs.begin(), m_household_size_fracs.end());
        // plus 1 because discrete_dist returns numbers between 0 and (m_household_size_fracs.size() - 1)
        // we need numbers between 1 and m_household_size_fracs.size()
        return (unsigned int)(m_rng.GetGenerator(distr)() + 1);
}

const vector<double>& PopulationGenerator::GetRandomModelHouseholdOfSize(unsigned int size)
{
    const auto& households = m_geogrid.GetModelHouseholds().at(size);
    trng::uniform_int_dist distr(0,(int) households.size());
    return households[m_rng.GetGenerator(distr)()];
}

double PopulationGenerator::GetRandomAge(Fractions category)
{
        switch (category) {
        case Fractions::SCHOOLED : // [3, 18)
                return (double)m_rng.GetGenerator(trng::uniform_dist<double>(3.0, 18.0))();
        case Fractions::YOUNG : // [18, 26)
                return (double)m_rng.GetGenerator(trng::uniform_dist<double>(18.0, 26.0))();
        case Fractions::MIDDLE_AGED : // [26, 65)
                return (double)m_rng.GetGenerator(trng::uniform_dist<double>(26.0, 65.0))();
        case Fractions::TODDLERS : // [0, 3)
                return (double)m_rng.GetGenerator(trng::uniform_dist<double>(0.0, 3.0))();
        case Fractions::OLDIES : // [65, 81), cause maximum age according to Age.h is 80...
                // gotta improve this since we would need [65, 80] but not with a uniform distribution...
                // because the chances you become older get smaller and smaller right?
                return (double)m_rng.GetGenerator(trng::uniform_dist<double>(65.0, 81.0))();
        default:
                // throw an exception here instead?
                return (double)m_rng.GetGenerator(trng::uniform_dist<double>(0.0, 81.0))();
        }
}

City& PopulationGenerator::GetRandomCity()
{
        trng::discrete_dist distr(m_city_pop_fracs.begin(), m_city_pop_fracs.end());
        return m_geogrid[m_city_ids[(unsigned int)m_rng.GetGenerator(distr)()]];
}

ContactPool* PopulationGenerator::GetRandomContactPool(const vector<Community*>& comms)
{
        if (comms.empty())
                return nullptr;

        trng::uniform_int_dist distr(0, comms.size());
        unsigned int           index = (unsigned int)m_rng.GetGenerator(distr)();
        auto&                  pools = comms.at(index)->GetContactPools();
        if (!pools.empty()) {
                trng::uniform_int_dist pdistr(0, pools.size());
                unsigned int           index2 = (unsigned int)m_rng.GetGenerator(pdistr)();
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
        return (const bool)m_rng.GetGenerator(distr)();
}

const bool PopulationGenerator::IsWorkingCommuter()
{
        return FlipUnfairCoin(m_geogrid.GetFraction(Fractions::COMMUTING_WORKERS));
}

const bool PopulationGenerator::IsStudentCommuter()
{
        return FlipUnfairCoin(m_geogrid.GetFraction(Fractions::COMMUTING_STUDENTS));
}

const bool PopulationGenerator::IsStudent() { return FlipUnfairCoin(m_geogrid.GetFraction(Fractions::STUDENTS)); }

const bool PopulationGenerator::IsActive() { return FlipUnfairCoin(m_geogrid.GetFraction(Fractions::ACTIVE)); }

ContactPool* PopulationGenerator::AssignWorkerAtRandom(City& origin)
{
        if (IsWorkingCommuter()) {
                City& city = GetRandomCommutingCity(origin);
                return GetRandomContactPool(city.GetCommunitiesOfType(CommunityType::Id::Work));
        } else
                return GetRandomContactPool(GetRandomCommunities(origin, CommunityType::Id::Work));
}

void PopulationGenerator::GeneratePerson(const double& age, const unsigned int hid, const unsigned int scid,
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
        unsigned int schoolid = (school) ? school->GetID() : 0;
        unsigned int workid   = (workplace) ? workplace->GetID() : 0;
        unsigned int pcid     = (primcomm) ? primcomm->GetID() : 0;
        pop.CreatePerson(m_id_generator++, age, hid, schoolid, workid, pcid, scid);
        Person* person = &pop.back();
        // Add the person to the contactpools, if any...
        if (school)
                school->AddMember(person);
        if (workplace)
                workplace->AddMember(person);
        if (primcomm)
                primcomm->AddMember(person);
}

void PopulationGenerator::GenerateHousehold(unsigned int size, City& city)
{
        auto&        pop           = *m_geogrid.GetPopulation();
        auto&        pool_sys      = pop.GetContactPoolSys();
        Household * the_household;
#pragma omp critical
    {
        the_household = &city.AddHousehold(pool_sys); // Returns a reference to the new household...
    }
        unsigned int hid           = the_household->GetID();

        ContactPool* seccomm = GetRandomContactPool(GetRandomCommunities(city, CommunityType::Id::Secondary));
        // Meaning you always get assigned to a community?
        auto scid = static_cast<unsigned int>((seccomm) ? seccomm->GetID() : 0);
        auto household = GetRandomModelHouseholdOfSize(size);
        for (unsigned int i = 0; i < household.size(); i++)
        {

            auto age = household.begin();
            advance(age, i);
                if( m_geogrid.UsingRandomAges() ) {
                    Fractions category = get_category(*age);
                    *age = GetRandomAge(category);
                }
                //if( model_household.size() == 1 and category == Fractions::SCHOOLED)
                //    cout << "Lonely minor of model age=" << model_household[i] << " has now received age=" << age << endl;
#pragma omp critical(a)
            {
                GeneratePerson(*age, hid, scid, pop, city);
           }
#pragma omp critical(b)
            {
                the_household->AddMember(&pop.back());
           }
                if (seccomm)
#pragma omp critical(c)
            {
                seccomm->AddMember(&pop.back());
            }
        }
}

void PopulationGenerator::GeneratePopulation()
{
        // TODO: currently it takes about 20sec to generate 4.3 million people,
        // TODO: this should be improved even more if possible...

        cout << "Starting population generation..." << endl;
        const clock_t      begin_time     = clock();
        const unsigned int max_population = m_geogrid.GetTotalPop();
        long long int      remaining_population = max_population; // long long to make sure the unsigned int fits...
        int thread_num = omp_get_max_threads(); // attempt to divide the work between threads.

// Static schedule to control the variance on our random elements -> but reduces performance
#pragma omp parallel for firstprivate(remaining_population) schedule(static)
        for(unsigned int i = 0; i < thread_num; i++) {
            auto this_thread_pop = remaining_population / thread_num;
            while (this_thread_pop > 0) {

                City &city = GetRandomCity();
                auto household_size = GetRandomHouseholdSize();
                // if the population has to be exact according to the one that we read on the file about cities
                // but this will effect our discrete distribution
                // Raphael@everyone, true, but the effect is insignificant given we have enough households...
                if (this_thread_pop - household_size < 0)
                    household_size = this_thread_pop;

                GenerateHousehold(household_size, city);

                this_thread_pop -= household_size;
            }
        }
        cout << "Done generating population, time needed = " << double(clock() - begin_time) / CLOCKS_PER_SEC << endl;
        SurveySeeder(m_geogrid.GetConfigPtree(), m_rng).Seed(m_geogrid.GetPopulation());
}

const vector<Community*>& PopulationGenerator::GetRandomCommunities(const City& city, const CommunityType::Id& community_type)
{
        unsigned int search_radius = m_geogrid.GetInitialSearchRadius();
        REQUIRE(search_radius > 0, "Initial search radius of the GeoGrid must be bigger than 0.");
        while (search_radius != 0) {
                const vector<City*>& near_cities = m_geogrid.GetCitiesWithinRadiusWithCommunityType(city, search_radius, community_type);
                if (!near_cities.empty()) {
                        auto index = (unsigned int)m_rng.GetGenerator(trng::uniform_int_dist(0, near_cities.size()))();
                        return near_cities[index]->GetCommunitiesOfType(community_type);
                }
                search_radius <<= 1; // equivalent to multiplying by 2
        }
        // throw exception here instead?
        return const_cast<City&>(city).GetCommunitiesOfType(community_type);
}

std::vector<Community*> PopulationGenerator::GetNearestColleges(const City& origin)
{
        vector<Community*>   result;
        const vector<City*>& cities(m_geogrid.GetCitiesWithCollege());
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
        const vector<double>& distribution =
            student ? m_student_commuting_fracs[origin.GetId()] : m_worker_commuting_fracs[origin.GetId()];
        trng::discrete_dist distr(distribution.begin(), distribution.end());
        auto                index = (const unsigned int)m_rng.GetGenerator(distr)();
        const unsigned int  id    = student ? m_college_ids.at(index) : m_city_ids.at(index);
        return m_geogrid.GetCities().at(id);
}

} // namespace stride
