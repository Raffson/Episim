//
// Created by Nishchal Shrestha on 20/03/2018.
//

#include "PopulationGenerator.h"

using namespace std;

namespace popgen {

unsigned int PopulationGenerator::m_id_generator = 1;

PopulationGenerator::PopulationGenerator(geogen::GeoGrid& geogrid)
    : m_geogrid(geogrid)
{
        InitializeHouseholdFractions();
        InitializeCommutingFractions();
        InitializeCityPopFractions();
}

geogen::Fractions GetCategory(const double& age)
{
        //Same ordering of if-else if as in GeoGrid::GetMainFractions, for the same reasons...
        if (age >= 26 and age < 65)
            return geogen::Fractions::OLD_WORKERS;
        else if (age >= 3 and age < 18)
            return geogen::Fractions::SCHOOLED;
        else if (age >= 18 and age < 26)
            return geogen::Fractions::YOUNG_WORKERS;
        else if (age >= 65)
            return geogen::Fractions::OLDIES;
        else
            return geogen::Fractions::TODDLERS;
}

void PopulationGenerator::InitializeHouseholdFractions()
{
        auto                            households = m_geogrid.GetModelHouseholds(); // get the model, model should be const...
        map<unsigned int, unsigned int> sizes;
        //The next map 'ages' will map a household's size to a 2nd map cotaining a Fraction type
        // which maps to the number people that belong to an age-category...
        // Thus we'll only be using SCHOOLED, YOUNG_WORKERS, OLD_WORKERS, TODDLERS & OLDIES from the Fractions enum...
        map<unsigned int, map<geogen::Fractions, double>> ages;
        //The next map 'total_ages' will simply map a household's 'size'
        // to the total amount of people that belong to a household with 'size'
        map<unsigned int, unsigned int> total_ages;
        for (auto& household : households) // count households with size x
        {
                sizes[household.size()] += 1;
                total_ages[household.size()] += household.size();
                for (auto& age : household)
                    ages[household.size()][GetCategory(age)] += 1;
        }

        double totalhhs = households.size();
        for (auto& elem : sizes) // household-count with size x divided by total nr of households (calculate fraction)
                m_household_size_fracs.emplace_back(elem.second / totalhhs);
        for (auto& elem : ages)
        {
                vector<double> composition_fracs;
                for(auto& cat : geogen::AgeList)
                        composition_fracs.emplace_back(elem.second[cat] / total_ages[elem.first]);
                m_household_comp_fracs.emplace(elem.first, composition_fracs);
        }
}

void PopulationGenerator::InitializeCommutingFractions()
{
        for (auto& cityA : m_geogrid.GetCities()) // for each cityA...
        {
                vector<double> distribution;
                double         commutersA = cityA.second.GetTotalOutCommutersCount(); // get total out-commuters from cityA
                for (auto& cityB : m_geogrid.GetCities()) // calculate the chance to commute from cityA to cityB
                {
                        // We don't want local commuting
                        if (cityA.first != cityB.first)
                                distribution.push_back(cityA.second.GetOutCommuting().at(cityB.first) / commutersA);
                        else // just push a 0, this will make sure this particular index can't be chosen...
                                distribution.push_back(0);
                }
                m_commuting_fracs[cityA.first] = distribution; // add the commuting distribution for cityA
        }
}

void PopulationGenerator::InitializeCityPopFractions()
{
        double totalpop = m_geogrid.GetTotalPop();
        for (auto& city : m_geogrid.GetCities())
                m_city_pop_fracs[city.second.GetId()] = city.second.GetPopulation() / totalpop;
}

unsigned int PopulationGenerator::GetRandomHouseholdSize()
{
        trng::discrete_dist distr(m_household_size_fracs.begin(), m_household_size_fracs.end());
        // plus 1 because discrete_dist returns numbers between 0 and (m_household_size_fracs.size() - 1)
        // we need numbers between 1 and m_household_size_fracs.size()
        return (unsigned int)(geogen::generator.GetGenerator(distr)() + 1);
}

double PopulationGenerator::GetRandomAge(unsigned int hhsize)
{
        //perhaps refractor and keep popfracs as a member?
        //->well after computing household composition fractions, we don't really need this anymore...
        //   depending on the size of the household we have a different age-distribution...
        //vector<double> popfracs;
        //m_geogrid.GetAgeFractions(popfracs);

        trng::discrete_dist distr(m_household_comp_fracs[hhsize].begin(), m_household_comp_fracs[hhsize].end());
        unsigned int        category = (unsigned int) geogen::generator.GetGenerator(distr)();

        switch (category) {
        case 0: {                                     // [3, 17]
                trng::uniform_dist<double> distr2(3.0, 18.0); // generates number between [a, b)
                return (double)geogen::generator.GetGenerator(distr2)();
        }
        case 1: { // [18, 25]
                trng::uniform_dist<double> distr2(18.0, 26.0);
                return (double)geogen::generator.GetGenerator(distr2)();
        }
        case 2: { // [26, 64]
                trng::uniform_dist<double> distr2(26.0, 65.0);
                return (double)geogen::generator.GetGenerator(distr2)();
        }
        case 3: { // [0, 2]
                trng::uniform_dist<double> distr2(0.0, 3.0);
                return (double)geogen::generator.GetGenerator(distr2)();
        }
        case 4: { // [65, 80], cause maximum age according to Age.h is 80...
                // gotta improve this since we would need [65, 80] but not with a uniform distribution...
                // because the chances you become older get smaller and smaller right?
                trng::uniform_dist<double> distr2(65.0, 81.0);
                return (double)geogen::generator.GetGenerator(distr2)();
        }
        default: {
                // cerr << "Bad random number was generated..." << endl;
                // what else can we do here? perhaps generate a nunber in the entire range?
                // doing this cause otherwise compiler will generate warning: control may reach end of non-void function
                // but in fact we should throw an exception here...
                trng::uniform_dist<double> distr2(0.0, 81.0);
                return (double)geogen::generator.GetGenerator(distr2)();
        }
        }
}

geogen::City& PopulationGenerator::GetRandomCity(const vector<unsigned int>& ids, const vector<double>& fracs)
{
    trng::discrete_dist distr(fracs.begin(), fracs.end());
    return m_geogrid[ids[(unsigned int)geogen::generator.GetGenerator(distr)()]];
}

stride::ContactPool* PopulationGenerator::GetRandomCommunityContactPool(const geogen::CommunityType& type,
                                                                        vector<geogen::Community*>& comms,
                                                                        const bool commuting)
{
        //TODO: do whatever we gotta do if it's a commuter

        if( comms.empty() ) return nullptr;

        trng::uniform_int_dist distr(0, comms.size());
        unsigned int index = (unsigned int) geogen::generator.GetGenerator(distr)();
        vector<stride::ContactPool>* pools = &comms[index]->GetContactPools();
        if( type == geogen::CommunityType::College and commuting )
        {
                //TODO: commuting student, now what?
                // well the pools will need te be adjusted...
        }
        else if( type == geogen::CommunityType::Work and commuting )
        {
                //TODO: commuting worker, now what?
                // well the pools will need te be adjusted...
        }
        if( pools and !pools->empty() )
        {
            trng::uniform_int_dist pdistr(0, pools->size());
            unsigned int index = (unsigned int) geogen::generator.GetGenerator(pdistr)();
            return &((*pools)[index]);
        }
        else return nullptr;
}

// Unfair, unless you pass frac=0.5
const bool FlipUnfairCoin(const double& frac)
{
        vector<double> fracs;
        fracs.push_back(1 - frac);
        fracs.push_back(frac);
        trng::discrete_dist distr(fracs.begin(), fracs.end());
        return (const bool)geogen::generator.GetGenerator(distr)();
}

const bool PopulationGenerator::IsWorkingCommuter() { return FlipUnfairCoin(m_geogrid.GetFraction(geogen::Fractions::COMMUTING_WORKERS)); }

const bool PopulationGenerator::IsStudentCommuter() { return FlipUnfairCoin(m_geogrid.GetFraction(geogen::Fractions::COMMUTING_STUDENTS)); }

const bool PopulationGenerator::IsStudent() { return FlipUnfairCoin(m_geogrid.GetFraction(geogen::Fractions::STUDENTS)); }

const bool PopulationGenerator::IsActive() { return FlipUnfairCoin(m_geogrid.GetFraction(geogen::Fractions::ACTIVE)); }

void PopulationGenerator::GeneratePerson(const double& age, const unsigned int hid,
                                         const unsigned int pcid, stride::Population& pop, geogen::City& city)
{
        //need the following:
        // unsigned int id, double age, unsigned int household_id, unsigned int school_id,
        // unsigned int work_id, unsigned int primary_community_id, unsigned int secondary_community_id,
        // Health health, const ptree& belief_pt, double risk_averseness
        //currently i have no clue what to do with risk_averseness,
        // so i'm just assigning it to 0...

        //TODO: gotta refactor because this is becoming too big & ugly...

        geogen::Fractions category = GetCategory(age);
        stride::ContactPool* school = nullptr;
        stride::ContactPool* workplace = nullptr;
        vector<geogen::Community*> seccomms;
        GetCommunitiesOfRandomNearbyCity(city, geogen::CommunityType::Secondary, seccomms);
        stride::ContactPool* seccomm = GetRandomCommunityContactPool(geogen::CommunityType::Secondary, seccomms);
        if( category == geogen::Fractions::SCHOOLED ) { // [3, 18)
            vector<geogen::Community*> schools;
            GetCommunitiesOfRandomNearbyCity(city, geogen::CommunityType::School, schools);
            school = GetRandomCommunityContactPool(geogen::CommunityType::School, schools);
        }
        else if( category == geogen::Fractions::YOUNG_WORKERS ) { // [18, 26)
            //first check if we have a student or not...
            if( IsStudent() ) {
                vector<geogen::Community*> colleges;
                GetNearestCollege(city, colleges);
                school = GetRandomCommunityContactPool(geogen::CommunityType::College, colleges, IsStudentCommuter());
            }
            else if( IsActive() ) {
                vector<geogen::Community*> workplaces;
                GetCommunitiesOfRandomNearbyCity(city, geogen::CommunityType::Work, workplaces);
                workplace = GetRandomCommunityContactPool(geogen::CommunityType::Work, workplaces, IsWorkingCommuter());
            }
        }
        else if( category == geogen::Fractions::OLD_WORKERS ) { // [26, 65)
            if( IsActive() ) {
                vector<geogen::Community*> workplaces;
                GetCommunitiesOfRandomNearbyCity(city, geogen::CommunityType::Work, workplaces);
                workplace = GetRandomCommunityContactPool(geogen::CommunityType::Work, workplaces, IsWorkingCommuter());
            }
        }
        else if( category == geogen::Fractions::TODDLERS ) { // [0, 3)
            //nothing?
        }
        else if( category == geogen::Fractions::OLDIES ) { // [65, 80], cause maximum age according to Age.h is 80...
            //nothing?
        }
        unsigned int schoolid = (school) ? school->GetID() : 0;
        unsigned int workid = (workplace) ? workplace->GetID() : 0;
        unsigned int scid = (seccomm) ? seccomm->GetID() : 0;
        pop.CreatePerson(m_id_generator++, age, hid, schoolid, workid, pcid, scid,
                     stride::Health(), m_geogrid.GetBelief(), 0.0);
        stride::Person* person = &pop.back();
        //Add the person to the contactpools, if any...
        if( school ) school->AddMember(person);
        if( workplace ) workplace->AddMember(person);
        if( seccomm ) seccomm->AddMember(person);
}

void PopulationGenerator::GenerateHousehold(unsigned int size, geogen::City& city)
{
        // TODO if we generate somebody less than 18 y then s/he should be accompanied by an adult?
        // update: turns out the household file has a very slim chance that a person between 3y en 18y old
        //    can be living alone... we're gonna have to notify the professor next time we get feedback...

        auto& pop = m_geogrid.GetPopulation();
        auto& the_household = city.AddHousehold(); //Returns a reference to the new household...
        unsigned int hid = the_household.GetID();

        vector<geogen::Community*> primcomms;
        GetCommunitiesOfRandomNearbyCity(city, geogen::CommunityType::Primary, primcomms);
        stride::ContactPool* primcomm = GetRandomCommunityContactPool(geogen::CommunityType::Primary, primcomms);
        //Meaning you always get assigned to a community?
        unsigned int pcid = (primcomm) ? primcomm->GetID() : 0;

        for (unsigned int i = 0; i < size; i++)
        {
                double age = GetRandomAge(size);
                GeneratePerson(age, hid, pcid, pop, city);
                the_household.AddMember(&pop.back());
                if( primcomm ) primcomm->AddMember(&pop.back());
        }
}

void PopulationGenerator::GeneratePopulation()
{
        //TODO: currently it takes about a minute to generate the total population,
        //TODO: this should be improved even more if possible...

        cout << "Starting population generation..." << endl;
        const clock_t begin_time = clock();
        const unsigned int max_population = m_geogrid.GetTotalPop();
        long long int remaining_population = max_population; //long long to make sure the unsigned int fits...

        vector<unsigned int> city_ids; // using boost::copy to copy all keys from m_city_pop_fracs into this vector...
        //city_ids can also be used for the commuting fractions since they are ordered the same,
        // this means we can save space by replacing some maps with regular vectors...
        vector<double> city_fracs; // using boost::copy to copy all values from m_city_pop_fracs into this vector...
        boost::copy(m_city_pop_fracs | boost::adaptors::map_keys, std::back_inserter(city_ids));
        boost::copy(m_city_pop_fracs | boost::adaptors::map_values, std::back_inserter(city_fracs));

        unsigned int counter = 0;
        while (remaining_population > 0) {
                geogen::City& city = GetRandomCity(city_ids, city_fracs);
                auto household_size = GetRandomHouseholdSize();

                // if the population has to be exact according to the one that we read on the file about cities
                // but this will effect our discrete distribution
                // Raphael@everyone, true, but the effect is insignificant given we have enough households...
                if( remaining_population - household_size < 0 )
                        household_size = remaining_population;
                GenerateHousehold(household_size, city);
                remaining_population -= household_size;
                counter += household_size;
                if( counter >= 100000 ) {
                    cout << "Mark 100000, " << remaining_population << " remaining..." << endl;
                    counter -= 100000;
                }
        }
        cout << "Done generating population, time needed = " << double(clock() - begin_time) /  CLOCKS_PER_SEC << endl;
}

void PopulationGenerator::GetCommunitiesOfRandomNearbyCity(const geogen::City& city,
                                                           const geogen::CommunityType& community_type,
                                                           vector<geogen::Community*>& result)
{
        unsigned int search_radius = m_geogrid.GetInitialSearchRadius();
        unsigned int avgcommsize = m_geogrid.GetAvgSize(geogen::Sizes::COMMUNITIES);
        const bool filter = (community_type == geogen::CommunityType::Primary
                             or community_type == geogen::CommunityType::Secondary);
        while (result.empty()) {
                const vector<geogen::City*>& near_cities = m_geogrid.GetCitiesWithinRadius(city, search_radius);
                //Two approaches, either we choose a random city and only take those communities,
                // or we take all commuties of the nearby cities <-- very slow, took about 12-13mins...
                // question remain whether we're allowed to use this first approach...
                //update: in case we use the first approach, this function must be called for each person,
                // not for each household which was the case...
                // this first approach in combination with pre-classified distances is way faster,
                // specifically about 12x faster...
                if( !near_cities.empty() ) {
                        trng::uniform_int_dist distr(0, near_cities.size());
                        unsigned int index = (unsigned int) geogen::generator.GetGenerator(distr)();
                        result = near_cities[index]->GetCommunitiesOfType(community_type, avgcommsize, filter);
                        return;
                }
                /*for (auto& a_city : near_cities) {
                        for (auto& a_community : a_city->GetCommunitiesOfType(community_type)) {
                                if( (community_type == geogen::CommunityType::Primary
                                    or community_type == geogen::CommunityType::Secondary)
                                    and a_community->GetSize() > avgcommsize )
                                        continue;
                                result.emplace_back(a_community);
                        }
                }*/
                search_radius <<= 1; //equivalent to multiplying by 2
        }
}

void PopulationGenerator::GetNearestCollege(const geogen::City &origin, std::vector<geogen::Community *> &result)
{
    const vector<geogen::City*>& cities(m_geogrid.GetCitiesWithCollege());
    double nearest = numeric_limits<double>::max();
    unsigned int nindex = 0;
    for (unsigned int i=0; i < cities.size(); i++) {
        double distance = cities[i]->GetCoordinates().GetDistance(origin.GetCoordinates());
        if (distance < nearest) {
            nearest = distance;
            nindex = i;
        }
    }
    for( auto college : cities[nindex]->GetCommunitiesOfType(geogen::CommunityType::College) )
        result.emplace_back(college);
}

// Quick refractor, will need to adjust the return type to vector<Person*>
// while using stride's Person class instead of the current struct...
vector<stride::Person*> PopulationGenerator::GetSchoolAttendants(geogen::City& city) //deprecated...
{

        vector<stride::Person*> school_attendants;
        /*
        for (auto& a_household : city->GetHouseholds()) {
                vector<shared_ptr<stride::Person>> current_school_attendants;
                a_household->GetSchoolAttendants(current_school_attendants); //GetSchoolAttendants may be a bad name, creating confusion...
                //we could also leave the next loop if we'd simply append schoolkids to 'school_attendants'
                // instead of working with 'current_school_attendants'
                for (auto a_school_attendant : current_school_attendants) {
                        school_attendants.push_back(a_school_attendant);
                }
        }
        */
        return school_attendants;
}

void PopulationGenerator::AssignToSchools() //deprecated...
{
    /*
        // Collecting all the school attendants from the city
        for (auto& a_city : m_geogrid.GetCities()) {
                vector<shared_ptr<stride::Person>> school_attendants = GetSchoolAttendants(a_city.second);

                // Search schools within 10km radius otherwise double the radius untill we find schools
                auto contact_pools = GetNearbyContactPools(*(a_city.second), geogen::CommunityType::School);
                //change name to 'GetNearbyContactPoolsOfType' ???

                // Select a school randomly for every school attendants
                for (auto& a_school_attendant : school_attendants) {
                        // choose random households to be assigned to the city <- wait what?
                        // how about we finish up this function?
                        trng::uniform_int_dist distr(0, contact_pools.size());
                        auto           index = (unsigned int) geogen::generator.GetGenerator(distr)();
                        // TODO use stride::Person class
                        // contact_pools.at(index)->AddMember(a_school_attendant);

                        // this cout actually suppresses the warnings as well...
                        //cout << a_school_attendant.age << " has been added to contact_pool " << index << endl;
                }
        }
    */
}

//not sure if we need this...
vector<stride::ContactPool*> PopulationGenerator::GetContactPoolsOfColleges()
{
        vector<stride::ContactPool*> result;
        for (auto& a_city : m_geogrid.GetCitiesWithCollege()) {
                for (auto& a_comunity : a_city->GetCommunitiesOfType(geogen::CommunityType::College)) {
                        for (auto& pool : a_comunity->GetContactPools())
                                result.emplace_back(&pool);
                }
        }
        return result;
}

void PopulationGenerator::AssignToColleges() //deprecated...
{
    /*
        auto contact_pools = GetContactPoolsOfColleges();

        vector<shared_ptr<stride::Person>> students;
        for (auto& a_city : m_geogrid.GetCities()) {
                for (auto& hh : a_city.second->GetHouseholds()) {
                        hh->GetCollegeStudents(students);
                }
        }

        for (auto& a_student : students) { //Gotta integrate commuting decently... I think everything must happen in the first for-loop, assigning on the fly...
                // it is chosen randomly right now
                // must look at other factors like possiblity to commute
                // rate of studying from home and so
                trng::uniform_int_dist distr(0, contact_pools.size());
                unsigned int           index = (unsigned int)geogen::generator.GetGenerator(distr)();
                cout << a_student.age << " is added to contact pool " << index << endl;
        }
    */
}

// This function may be forgetting about students, however irl we also have working students...
// though they would be working part-time, we should ask ourselves (or the professor) how this needs to be handled...
// Nishchal@everyone If I am not mistaken it was said during one of the classes that we don't have to take part-time
// students into account
// Raphael@Nishchal, roger that! but still, at this point I believe we're mixing students and workers,
// i.e. if I understood the code correctly... I believe Household::GetPossibleWorkers needs to check work-id != 1
vector<stride::Person*> PopulationGenerator::GetActives(geogen::City& city) //deprecated
{

        vector<stride::Person*> actives;
/*
        for (auto& a_household : city->GetHouseholds()) {
                vector<shared_ptr<stride::Person>> possible_workers;
                a_household->GetPossibleWorkers(possible_workers);
                for (auto& a_possible_worker : possible_workers) {
                        if (this->IsActive()) {
                                actives.push_back(a_possible_worker);
                        }
                }
        }
*/
        return actives;
}

//this we still need... although it may need be reworked slightly...
geogen::City& PopulationGenerator::GetRandomCommutingCity(geogen::City& origin,
                                                          const vector<unsigned int>&  city_ids)
{
        const vector<double>& distribution = m_commuting_fracs[origin.GetId()];
        trng::discrete_dist   distr(distribution.begin(), distribution.end());
        auto  index = (const unsigned int)geogen::generator.GetGenerator(distr)();
        auto  id    = (const unsigned int)city_ids.at(index);
        return m_geogrid.GetCities().at(id);
}

void PopulationGenerator::AssignToWorkplaces() //deprecated...
{
    /*
        vector<int> city_ids; // using boost::copy to copy all keys from cities into this vector...
        boost::copy(m_geogrid.GetCities() | boost::adaptors::map_keys, std::back_inserter(city_ids));
        for (auto& a_city : m_geogrid.GetCities()) {
                for (auto an_active : GetActives(a_city.second)) {
                        vector<shared_ptr<stride::ContactPool>> contact_pools;
                        if (!IsWorkingCommuter()) {
                                contact_pools = GetNearbyContactPools(*(a_city.second), geogen::CommunityType::Work);

                        }

                        // Commuting workers
                        else {
                                // Raphael@Nishchal, so GetRandomCommutingCity is being called for every active worker,
                                // meaning that the distributions for all active workers from a particular city should
                                // be the same... therefore I believe we should initialize this in the constructor using
                                // a map<cityID, outCommutingDistributionOfCityWith-cityID> if my reasoning is
                                // incorrect, we will revert, however I'm pretty sure we're doing too much work now...
                                // i.e. less time-complexity, but more space-complexity

                                // Nishchal@Raphael I agree with you. We can save time doing this (with some space-
                                //complexity) -> Raphael@Nishchal, copy that! we'll leave it like this unless we
                                // run into trouble (which I strongly doubt)

                                //Nishchal@everyone I wonder, can't we do the same for distances between cities also.
                                //Atm we are repeating calculation of distances between cities more than once
                                //Raphael@Nishchal, affirmative! however this refractor will require a little bit more
                                // work which I'd rather postpone atm, we should focus on verifying the correctness
                                // of all our code right now, indicating all possible issues so we can discuss them...
                                // second thought, this would require quite some space...
                                auto workplace_city = GetRandomCommutingCity(*(a_city.second), city_ids);
                                auto workplaces     = workplace_city->GetCommunitiesOfType(geogen::CommunityType::Work);

                                // Adding possible contactpools to be randomly chosen
                                for (auto& a_workplace : workplaces) {
                                        auto current_cp = a_workplace->GetContactPools();
                                        contact_pools.insert(contact_pools.end(), current_cp.begin(), current_cp.end());
                                }
                                cout << "commuting....";
                        }

                        trng::uniform_int_dist distr(0, contact_pools.size());
                        auto                   index = (unsigned int) geogen::generator.GetGenerator(distr)();
                        cout << an_active.age << " is added to workplace " << index << endl;
                }
        }
     */
}

void PopulationGenerator::AssignToCommunity() //deprecated...
{
    /*
        for (auto& a_city : m_geogrid.GetCities()) {
                auto contact_pools = GetNearbyContactPools(*(a_city.second), geogen::CommunityType::Primary);
                for (auto& a_household : a_city.second->GetHouseholds()) {
                        for (auto& a_person : a_household->GetMembers()) {
                                // assign the person to a random contactPool
                                trng::uniform_int_dist distr(0, contact_pools.size());
                                auto           index = (unsigned int) geogen::generator.GetGenerator(distr)();

                                // TODO the member has to be a stride::Person to be added to stride::ContactPool

                                // nearest_contact_pools.at(index)->AddMember(a_person);
                                // cout <<"A person of age "<< a_person.age << " is added to contact pool at index " <<
                                // index <<endl;

                                // TODO check the community for that contactpool
                                // TODO if the limit is crossed remove from the list of the possible
                                // communities(contactpools)
                                //}
                                cout << a_person.age << " is added to contact pool at index " << index << endl;
                        }
                }
        }
    */
}

void PopulationGenerator::AssignAll() //deprecated...
{
        // TODO Must test/make sure that households are assigned before others
        AssignToSchools();
        AssignToColleges();
        AssignToWorkplaces();
        AssignToCommunity();
}

} // namespace popgen
