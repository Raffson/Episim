//
// Created by Nishchal Shrestha on 20/03/2018.
//

#include "PopulationGenerator.h"

using namespace std;

namespace stride {

unsigned int PopulationGenerator::m_id_generator = 1;

PopulationGenerator::PopulationGenerator(GeoGrid& geogrid) : m_geogrid(geogrid)
{
        InitializeHouseholdFractions();
        InitializeCommutingFractions();
        InitializeCityPopFractions();
        m_rng = m_geogrid.GetRNG();
}

void PopulationGenerator::InitializeHouseholdFractions()
{
        auto households = m_geogrid.GetModelHouseholds();
        for (auto& elem : households)
                m_household_size_fracs.emplace_back(elem.second.size());
}

void PopulationGenerator::InitializeCommutingFractions()
{
        const double commuting_students_frac = m_geogrid.GetFraction(Fractions::COMMUTING_STUDENTS);
        const double student_frac            = m_geogrid.GetFraction(Fractions::STUDENTS);
        const double young_workers_frac      = m_geogrid.GetFraction(Fractions::YOUNG);
        const double student_commuters_frac  = commuting_students_frac * student_frac * young_workers_frac;
        for (auto& cityA : m_geogrid.GetCities()) // for each cityA...
        {
                m_city_ids.emplace_back(cityA.first);
                if (!cityA.second.GetColleges().empty())
                        m_college_ids.emplace_back(cityA.first);
                vector<double> worker_dist;
                vector<double> student_dist;
                double         commutersA = cityA.second.GetTotalOutCommutersCount();
                for (auto& cityB : m_geogrid.GetCities()) // calculate the chance to commute from cityA to cityB
                {
                        const bool has_college = !cityB.second.GetColleges().empty();
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
        for (auto& city : m_geogrid.GetCities())
                m_city_pop_fracs.emplace_back(city.second.GetPopulation() / totalpop);
}

unsigned int PopulationGenerator::GetRandomHouseholdSize()
{
        trng::discrete_dist distr(m_household_size_fracs.begin(), m_household_size_fracs.end());
        // plus 1 because discrete_dist returns numbers between 0 and (m_household_size_fracs.size() - 1)
        // we need numbers between 1 and m_household_size_fracs.size()
        return (unsigned int)(m_rng->GetGenerator(distr)() + 1);
}

vector<double> PopulationGenerator::GetRandomModelHouseholdOfSize(unsigned int size)
{
    const vector<vector<double>>& households = m_geogrid.GetModelHouseholds().at(size);
    trng::uniform_int_dist distr(0, households.size());
    return households[m_rng->GetGenerator(distr)()];
}

double PopulationGenerator::GetRandomAge(Fractions category)
{
        switch (category) {
        case Fractions::SCHOOLED : {                          // [3, 17]
                trng::uniform_dist<double> distr2(3.0, 18.0); // generates number between [a, b)
                return (double)m_rng->GetGenerator(distr2)();
        }
        case Fractions::YOUNG : { // [18, 25]
                trng::uniform_dist<double> distr2(18.0, 26.0);
                return (double)m_rng->GetGenerator(distr2)();
        }
        case Fractions::MIDDLE_AGED : { // [26, 64]
                trng::uniform_dist<double> distr2(26.0, 65.0);
                return (double)m_rng->GetGenerator(distr2)();
        }
        case Fractions::TODDLERS : { // [0, 2]
                trng::uniform_dist<double> distr2(0.0, 3.0);
                return (double)m_rng->GetGenerator(distr2)();
        }
        case Fractions::OLDIES : { // [65, 80], cause maximum age according to Age.h is 80...
                // gotta improve this since we would need [65, 80] but not with a uniform distribution...
                // because the chances you become older get smaller and smaller right?
                trng::uniform_dist<double> distr2(65.0, 81.0);
                return (double)m_rng->GetGenerator(distr2)();
        }
        default: {
                // cerr << "Bad random number was generated..." << endl;
                // what else can we do here? perhaps generate a nunber in the entire range?
                // doing this cause otherwise compiler will generate warning: control may reach end of non-void function
                // but in fact we should throw an exception here...
                trng::uniform_dist<double> distr2(0.0, 81.0);
                return (double)m_rng->GetGenerator(distr2)();
        }
        }
}

City& PopulationGenerator::GetRandomCity()
{
        trng::discrete_dist distr(m_city_pop_fracs.begin(), m_city_pop_fracs.end());
        return m_geogrid[m_city_ids[(unsigned int)m_rng->GetGenerator(distr)()]];
}

ContactPool* PopulationGenerator::GetRandomCommunityContactPool(vector<Community*>& comms)
{
        if (comms.empty())
                return nullptr;

        trng::uniform_int_dist distr(0, comms.size());
        unsigned int           index = (unsigned int)m_rng->GetGenerator(distr)();
        vector<ContactPool*>&  pools = comms[index]->GetContactPools();
        if (!pools.empty()) {
                trng::uniform_int_dist pdistr(0, pools.size());
                unsigned int           index2 = (unsigned int)m_rng->GetGenerator(pdistr)();
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
        return (const bool)m_rng->GetGenerator(distr)();
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
        vector<Community*> workplaces;
        if (IsWorkingCommuter()) {
                City& commuter_city = GetRandomCommutingCity(origin);
                workplaces          = commuter_city.GetCommunitiesOfType(CommunityType::Work);
        } else
                workplaces = GetCommunitiesOfRandomNearbyCity(origin, CommunityType::Work);
        return GetRandomCommunityContactPool(workplaces);
}

void PopulationGenerator::GeneratePerson(const double& age, const unsigned int hid, const unsigned int pcid,
                                         Population& pop, City& city)
{
        // need the following:
        // unsigned int id, double age, unsigned int household_id, unsigned int school_id,
        // unsigned int work_id, unsigned int primary_community_id, unsigned int secondary_community_id,
        // Health health, const ptree& belief_pt, double risk_averseness
        // currently i have no clue what to do with risk_averseness,
        // so i'm just assigning it to 0...

        // TODO: gotta refactor because this is becoming too big & ugly...

        Fractions          category  = get_category(age);
        ContactPool*       school    = nullptr;
        ContactPool*       workplace = nullptr;
        vector<Community*> seccomms = GetCommunitiesOfRandomNearbyCity(city, CommunityType::Secondary);
        ContactPool* seccomm = GetRandomCommunityContactPool(seccomms);
        if (category == Fractions::SCHOOLED) { // [3, 18)
                vector<Community*> schools = GetCommunitiesOfRandomNearbyCity(city, CommunityType::School);
                school = GetRandomCommunityContactPool(schools);
        } else if (category == Fractions::YOUNG) { // [18, 26)
                // first check if we have a student or not...
                if (IsStudent()) {
                        vector<Community*> colleges;
                        if (IsStudentCommuter()) {
                                City& commuter_city = GetRandomCommutingCity(city, true);
                                colleges            = commuter_city.GetCommunitiesOfType(CommunityType::College);
                        } else
                                GetNearestColleges(city, colleges);
                        school = GetRandomCommunityContactPool(colleges);
                } else if (IsActive())
                        workplace = AssignWorkerAtRandom(city);
        } else if (category == Fractions::MIDDLE_AGED) { // [26, 65)
                if (IsActive())
                        workplace = AssignWorkerAtRandom(city);
        }
        unsigned int schoolid = (school) ? school->GetID() : 0;
        unsigned int workid   = (workplace) ? workplace->GetID() : 0;
        unsigned int scid     = (seccomm) ? seccomm->GetID() : 0;
        pop.CreatePerson(m_id_generator++, age, hid, schoolid, workid, pcid, scid, Health(), m_geogrid.GetBelief(),
                         0.0);
        Person* person = &pop.back();
        // Add the person to the contactpools, if any...
        if (school)
                school->AddMember(person);
        if (workplace)
                workplace->AddMember(person);
        if (seccomm)
                seccomm->AddMember(person);
}

void PopulationGenerator::GenerateHousehold(unsigned int size, City& city)
{
        auto&        pop           = *m_geogrid.GetPopulation();
        auto&        pool_sys      = m_geogrid.GetContactPoolSys();
        auto&        the_household = city.AddHousehold(pool_sys); // Returns a reference to the new household...
        unsigned int hid           = the_household.GetID();

        vector<Community*> primcomms = GetCommunitiesOfRandomNearbyCity(city, CommunityType::Primary);
        ContactPool* primcomm = GetRandomCommunityContactPool(primcomms);
        // Meaning you always get assigned to a community?
        unsigned int pcid = (primcomm) ? primcomm->GetID() : 0;

        vector<double> model_household = GetRandomModelHouseholdOfSize(size);

        for (unsigned int i = 0; i < model_household.size(); i++) {
                double age = model_household[i];
                if( m_geogrid.UsingRandomAges() ) {
                    Fractions category = get_category(age);
                    age = GetRandomAge(category);
                }
                //if( model_household.size() == 1 and category == Fractions::SCHOOLED)
                //    cout << "Lonely minor of model age=" << model_household[i] << " has now received age=" << age << endl;
                GeneratePerson(age, hid, pcid, pop, city);
                the_household.AddMember(&pop.back());
                if (primcomm)
                        primcomm->AddMember(&pop.back());
        }
}

void PopulationGenerator::GeneratePopulation()
{
        // TODO: currently it takes about 30sec to generate 4.3 million people,
        // TODO: this should be improved even more if possible...

        cout << "Starting population generation..." << endl;
        const clock_t      begin_time     = clock();
        const unsigned int max_population = m_geogrid.GetTotalPop();
        long long int      remaining_population = max_population; // long long to make sure the unsigned int fits...

        unsigned int counter = 0;
        while (remaining_population > 0) {
                City& city           = GetRandomCity();
                auto  household_size = GetRandomHouseholdSize();

                // if the population has to be exact according to the one that we read on the file about cities
                // but this will effect our discrete distribution
                // Raphael@everyone, true, but the effect is insignificant given we have enough households...
                if (remaining_population - household_size < 0)
                        household_size = remaining_population;
                GenerateHousehold(household_size, city);
                remaining_population -= household_size;
                counter += household_size;
                if (counter >= 100000) {
                        cout << "Mark 100000, " << remaining_population << " remaining..." << endl;
                        counter -= 100000;
                }
        }
        cout << "Done generating population, time needed = " << double(clock() - begin_time) / CLOCKS_PER_SEC << endl;
}

vector<Community*> PopulationGenerator::GetCommunitiesOfRandomNearbyCity(const City& city, const CommunityType& community_type)
{
        unsigned int search_radius = m_geogrid.GetInitialSearchRadius();
        REQUIRE(search_radius > 0, "Initial search radius of the GeoGrid must be bigger than 0.");
        vector<Community*> result;
        while (result.empty() and search_radius != 0) {
                const vector<City*>& near_cities = m_geogrid.GetCitiesWithinRadiusWithCommunityType(city, search_radius, community_type);
                if (!near_cities.empty()) {
                        trng::uniform_int_dist distr(0, near_cities.size());
                        unsigned int           index = (unsigned int)m_rng->GetGenerator(distr)();
                        result = near_cities[index]->GetCommunitiesOfType(community_type);
                }
                /*for (auto& a_city : near_cities) {
                        for (auto& a_community : a_city->GetCommunitiesOfType(community_type)) {
                                if( (community_type == CommunityType::Primary
                                    or community_type == CommunityType::Secondary)
                                    and a_community->GetSize() > avgcommsize )
                                        continue;
                                result.emplace_back(a_community);
                        }
                }*/
                search_radius <<= 1; // equivalent to multiplying by 2
        }
        if( search_radius == 0 ) cout << "Result of 'GetCommunitiesOfRandomNearbyCity' is empty!" << endl;
        return result;
}

void PopulationGenerator::GetNearestColleges(const City& origin, std::vector<Community*>& result)
{
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
        for (auto college : cities[nindex]->GetCommunitiesOfType(CommunityType::College))
                result.emplace_back(college);
}

City& PopulationGenerator::GetRandomCommutingCity(City& origin, const bool student)
{
        const vector<double>& distribution =
            student ? m_student_commuting_fracs[origin.GetId()] : m_worker_commuting_fracs[origin.GetId()];
        trng::discrete_dist distr(distribution.begin(), distribution.end());
        auto                index = (const unsigned int)m_rng->GetGenerator(distr)();
        const unsigned int  id    = student ? m_college_ids.at(index) : m_city_ids.at(index);
        return m_geogrid.GetCities().at(id);
}

} // namespace stride
