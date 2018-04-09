//
// Created by Nishchal Shrestha on 20/03/2018.
//

#include "PopulationGenerator.h"

using namespace std;

namespace popgen {

PopulationGenerator::PopulationGenerator(geogen::GeoGrid& geogrid, unsigned int rad)
    : m_geogrid(geogrid), m_initial_search_radius(rad)
{
        InitializeHouseholdSizeFractions();
        InitializeCommutingFractions();
}

void PopulationGenerator::InitializeHouseholdSizeFractions()
{
        auto                            households = m_geogrid.GetModelHouseholds(); // get the model, model should be const...
        map<unsigned int, unsigned int> sizes;
        for (auto& household : households) // count households with size x
                sizes[household.size()] += 1;

        double totalhhs = households.size();
        for (auto& elem : sizes) // household-count with size x divided by total nr of households (calculate fraction)
                m_household_size_fracs.push_back(elem.second / totalhhs);
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

unsigned int PopulationGenerator::GetRandomHouseholdSize()
{
        trng::discrete_dist distr(m_household_size_fracs.begin(), m_household_size_fracs.end());
        // plus 1 because discrete_dist returns numbers between 0 and (m_household_size_fracs.size() - 1)
        // we need numbers between 1 and m_household_size_fracs.size()
        return (unsigned int)(geogen::generator.GetGenerator(distr)() + 1);
}

double PopulationGenerator::GetRandomAge()
{
        //perhaps refractor and keep popfracs as a member?
        vector<double> popfracs;
        m_geogrid.GetAgeFractions(popfracs);

        trng::discrete_dist distr(popfracs.begin(), popfracs.end());
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

// Unfair, unless you pass frac=0.5
bool FlipUnfairCoin(const double& frac)
{
        vector<double> fracs;
        fracs.push_back(1 - frac);
        fracs.push_back(frac);
        trng::discrete_dist distr(fracs.begin(), fracs.end());
        return (bool)geogen::generator.GetGenerator(distr)();
}

bool PopulationGenerator::IsWorkingCommuter() { return FlipUnfairCoin(m_geogrid.GetCommutingWorkersFrac()); }

//bool PopulationGenerator::IsStudentCommuter() { return FlipUnfairCoin(m_geogrid.GetCommutingStudentsFrac()); }

//bool PopulationGenerator::IsStudent() { return FlipUnfairCoin(m_geogrid.GetStudentFrac()); }

bool PopulationGenerator::IsActive() { return FlipUnfairCoin(m_geogrid.GetActiveFrac()); }

    /*
shared_ptr<Household> PopulationGenerator::GenerateHousehold(unsigned int size)
{
        // TODO if we generate somebody less than 18 y then s/he should be accompanied by an adult?
        // A household with 1 person won't be possible in that case
        // Raphael@everyone, not necessarily, we just need to check the age distribution for a household of size x
        // meaning we need a map sort of like m_commuting_fracs...
        // mapping the size of a household, i.e. 1,2,3,... (deduced from m_household_size_fracs)
        // to a distribution for age categories, i.e. a vector sort of like popfracs in GetRandomAge


        auto the_household = make_shared<Household>(0);
        for (unsigned int i = 0; i < size; i++) {

                shared_ptr<Person> a_person();
                the_household->AddMember(a_person);

                // cout << a_person.age << endl;
        }
        // cout << "------------------" << endl;
        return the_household;
}*/

void PopulationGenerator::AssignHouseholds()
{
    /*
        //Problems with (unsigned ints) & ints, and fuck the (unsigned int) casts,
        // get rid of auto cause if we leave out the casts we need to know what type we're dealing with...
        for (auto& a_city : m_geogrid.GetCities()) {
                const unsigned int max_population       = a_city.second->GetPopulation();
                auto remaining_population = max_population;

                while (remaining_population > 0) {
                        auto household_size = this->GetRandomHouseholdSize();

                        // if the population has to be exact according to the one that we read on the file about cities
                        // but this will effect our discrete distribution
                        // Raphael@everyone, true, but the effect is insignificant given we have enough households...
                        if( (int)(remaining_population - household_size) < 0 ) {
                                household_size = remaining_population;
                        }
                        auto hh = GenerateHousehold(household_size);
                        hh->SetCityID(a_city.second->GetId());
                        a_city.second->AddHousehold(hh);
                        remaining_population -= household_size;
                }
        }
     */
}

vector<geogen::City*> PopulationGenerator::GetCitiesWithinRadius(const geogen::City& origin,
                                                                 unsigned int radius, unsigned int last)
{
        // TODO to save time we can add distances between two cities in a map but will cost some space
        // This will take like A LOT of space.. not sure if it's worth it...
        vector<geogen::City*> result;
        for (auto a_city : m_geogrid.GetCities()) {
                double distance = a_city.second.GetCoordinates().GetDistance(origin.GetCoordinates());
                if (distance <= radius and distance >= last) {
                        result.push_back(&a_city.second);
                }
        }

        return result;
}

vector<stride::ContactPool*> PopulationGenerator::GetNearbyContactPools(const geogen::City&   city,
                                                                        geogen::CommunityType community_type)
{
        unsigned int                            search_radius = m_initial_search_radius;
        unsigned int                            last_radius   = 0;
        vector<stride::ContactPool*> result;

        // this while(true) loop creaps me the fuck out, thinking about a better solution...
        // still need a better solution....
        while (true) {
                vector<geogen::City*> near_cities = GetCitiesWithinRadius(city, search_radius, last_radius);
                for (auto& a_city : near_cities) {
                        for (auto& a_community : a_city->GetCommunitiesOfType(community_type)) {
                                for (auto& pool : a_community->GetContactPools() )
                                        result.emplace_back(&pool);
                        }
                }

                if (result.empty()) {
                        last_radius = search_radius;
                        search_radius *= 2;
                } else {
                        return result;
                }
        }
}

// Quick refractor, will need to adjust the return type to vector<Person*>
// while using stride's Person class instead of the current struct...
vector<stride::Person*> PopulationGenerator::GetSchoolAttendants(geogen::City& city)
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

void PopulationGenerator::AssignToSchools()
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

void PopulationGenerator::AssignToColleges()
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
vector<stride::Person*> PopulationGenerator::GetActives(geogen::City& city)
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

geogen::City& PopulationGenerator::GetRandomCommutingCity(geogen::City& origin,
                                                          const vector<int>&  city_ids)
{
        vector<double>      distribution = m_commuting_fracs[origin.GetId()];
        trng::discrete_dist distr(distribution.begin(), distribution.end());
        auto  index = (const unsigned int)geogen::generator.GetGenerator(distr)();
        auto  id    = (const unsigned int)city_ids.at(index);
        return m_geogrid.GetCities().at(id);
}

void PopulationGenerator::AssignToWorkplaces()
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

//Finish up this function... and again, fuck auto, fuck casts...
void PopulationGenerator::AssignToCommunity()
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

void PopulationGenerator::AssignAll()
{
        // TODO Must test/make sure that households are assigned before others
        AssignHouseholds();
        AssignToSchools();
        AssignToColleges();
        AssignToWorkplaces();
        AssignToCommunity();
}

} // namespace popgen
