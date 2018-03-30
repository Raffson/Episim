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
}

void PopulationGenerator::InitializeHouseholdSizeFractions()
{
        auto                            households = m_geogrid.GetModelHouseholds();
        map<unsigned int, unsigned int> sizes;
        for (auto& household : households)
                sizes[household->GetMembers().size()] += 1;

        double totalhhs = households.size();
        for (auto& elem : sizes)
                m_household_size_fracs.push_back(elem.second / totalhhs);
}

unsigned int PopulationGenerator::GetRandomHouseholdSize()
{
        trng::discrete_dist distr(m_household_size_fracs.begin(), m_household_size_fracs.end());
        // plus 1 because discrete_dist returns numbers between 0 and (m_household_size_fracs.size() - 1)
        // we need numbers between 1 and m_household_size_fracs.size()
        return (geogen::generator.GetGenerator(distr)() + 1);
}

unsigned int PopulationGenerator::GetRandomAge()
{
        vector<double> popfracs;
        popfracs.push_back(m_geogrid.GetSchooledFrac()); // [3, 17]
        popfracs.push_back(m_geogrid.GetWorkers1Frac()); // [18, 25]
        popfracs.push_back(m_geogrid.GetWorkers2Frac()); // [26, 64]
        popfracs.push_back(m_geogrid.GetToddlersFrac()); // [0, 2]
        popfracs.push_back(m_geogrid.GetOldiesFrac());   // [65, 123?]
        // cause oldest person ever lived was 122 years and 164 days according to wikipedia

        trng::discrete_dist distr(popfracs.begin(), popfracs.end());
        unsigned int        category = geogen::generator.GetGenerator(distr)();

        switch (category) {
        case 0: {                                     // [3, 17]
                trng::uniform_int_dist distr2(3, 18); // generates number between [a, b)
                return geogen::generator.GetGenerator(distr2)();
        }
        case 1: { // [18, 25]
                trng::uniform_int_dist distr2(18, 26);
                return geogen::generator.GetGenerator(distr2)();
        }
        case 2: { // [26, 64]
                trng::uniform_int_dist distr2(26, 65);
                return geogen::generator.GetGenerator(distr2)();
        }
        case 3: { // [0, 2]
                trng::uniform_int_dist distr2(0, 3);
                return geogen::generator.GetGenerator(distr2)();
        }
        case 4: { // [65, 123?]
                // gotta improve this since we would need [65, 123?] but not with a uniform distribution...
                // because the chances you become older get smaller and smaller right?
                trng::uniform_int_dist distr2(65, 123);
                return geogen::generator.GetGenerator(distr2)();
        }
        default: {
                // cerr << "Bad random number was generated..." << endl;
                // what else can we do here? perhaps generate a nunber in the entire range?
                // doing this cause otherwise compiler will generate warning: control may reach end of non-void function
                // but in fact we should throw an exception here...
                trng::uniform_int_dist distr2(0, 123);
                return geogen::generator.GetGenerator(distr2)();
        }
        }
}

bool FlipCoin(const double& frac)
{
        vector<double> fracs;
        fracs.push_back(1 - frac);
        fracs.push_back(frac);
        trng::discrete_dist distr(fracs.begin(), fracs.end());
        return (bool)geogen::generator.GetGenerator(distr)();
}

bool PopulationGenerator::IsWorkingCommuter() { return FlipCoin(m_geogrid.GetCommutingWorkersFrac()); }

bool PopulationGenerator::IsStudentCommuter() { return FlipCoin(m_geogrid.GetCommutingStudentsFrac()); }

bool PopulationGenerator::IsStudent() { return FlipCoin(m_geogrid.GetStudentFrac()); }

bool PopulationGenerator::IsActive() { return FlipCoin(m_geogrid.GetActiveFrac()); }

shared_ptr<Household> PopulationGenerator::GenerateHousehold(unsigned int size)
{
    auto the_household = make_shared<Household>();
    for(unsigned int i=0; i<size; i++){
        Person a_person;
        a_person.age = this->GetRandomAge();
        the_household->AddMember(a_person);
        cout << a_person.age << endl;
    }
        cout << "------------------" << endl;
    return the_household;
}

void PopulationGenerator::AssignHouseholds()
{

    for(auto& a_city: m_geogrid.GetCities()){
        const unsigned int max_population = a_city.second->GetPopulation();
        int remaining_population = (int) max_population;

        while(remaining_population > 0){
            unsigned int household_size = this->GetRandomHouseholdSize();

            //if the population has to be exact according to the one that we read on the file about cities
            //but this will effect our discrete distribution
            if(remaining_population - (int)household_size < 0){
                household_size = remaining_population;
            }
            a_city.second->AddHousehold(GenerateHousehold(household_size));
            remaining_population -= household_size;
        }
    }
}

vector<shared_ptr<geogen::City>> PopulationGenerator::GetCitiesWithinRadius(const geogen::City& origin,
                                                                            unsigned int radius, unsigned int last)
{
        // TODO to save time we can add distances between two cities in a map but will cost some space
        vector<shared_ptr<geogen::City>> result;
        for (auto a_city : m_geogrid.GetCities()) {
                double distance = GetDistance(a_city.second->GetCoordinates(), origin.GetCoordinates());
                if (distance <= radius and distance >= last) {
                        result.push_back(a_city.second);
                }
        }

        return result;
}

double PopulationGenerator::GetDistance(geogen::Coordinate c1, geogen::Coordinate c2)
{
        const double earths_radius = 6371.0; // in kilometer

        double phi1    = c1.latitude * M_PI / 180.0;  // latitude of the first coordinate converted to radian
        double phi2    = c2.latitude * M_PI / 180.0;  // latitude of the second coordinate converted to radian
        double lambda1 = c1.longitude * M_PI / 180.0; // longitude of the first coordinate converted to radian
        double lambda2 = c2.longitude * M_PI / 180.0; // longitude of the second coordinate converted to radian

        double delta_phi    = phi2 - phi1;
        double delta_lambda = lambda2 - lambda1;

        double a = sin(delta_phi / 2.0) * sin(delta_phi / 2.0) +
                   cos(phi1) * cos(phi2) * cos(phi1) * cos(phi2) * sin(delta_lambda / 2.0) * sin(delta_lambda / 2.0);

        double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));

        return earths_radius * c;
}

vector<shared_ptr<stride::ContactPool>> PopulationGenerator::GetNearbyContactPools(const geogen::City&   city,
                                                                                   geogen::CommunityType community_type)
{
        unsigned int                            search_radius = m_initial_search_radius;
        unsigned int                            last_radius   = 0;
        vector<shared_ptr<stride::ContactPool>> result;

        // this while(true) loop creaps me the fuck out, thinking about a better solution...
        while (true) {
                vector<shared_ptr<geogen::City>> near_cities = GetCitiesWithinRadius(city, search_radius, last_radius);
                for (auto& a_city : near_cities) {
                        for (auto& a_community : a_city->GetCommunitiesOfType(community_type)) {
                                vector<shared_ptr<stride::ContactPool>> compools = a_community->GetContactPools();
                                result.insert(result.end(), compools.begin(), compools.end());
                        }
                }

                if (result.size() == 0) {
                        last_radius = search_radius;
                        search_radius *= 2;
                } else {
                        return result;
                }
        }
}

// Quick refractor, will need to adjust the return type to vector<shared_ptr<Person>>
// while using stride's Person class instead of the current struct...
vector<Person> PopulationGenerator::GetSchoolAttendants(const shared_ptr<geogen::City>& city)
{
        vector<Person> school_attendants;
        for (auto& a_household : city->GetHouseholds()) {
                vector<Person> current_school_attendants;
                a_household->GetSchoolAttendants(current_school_attendants);
                for (auto a_school_attendant : current_school_attendants) {
                        school_attendants.push_back(a_school_attendant);
                }
        }
        return school_attendants;
}

void PopulationGenerator::AssignToSchools()
{
        // Collecting all the school attendants from the city
        for (auto& a_city : m_geogrid.GetCities()) {
                vector<Person> school_attendants = GetSchoolAttendants(a_city.second);

                // Search schools within 10km radius otherwise double the radius untill we find schools
                auto contact_pools = GetNearbyContactPools(*(a_city.second), geogen::CommunityType::School);

                // Select a school randomly for every school attendants
                // Raphael@Nishchal, if you don't need 'a_school_attendant', the use the following:
                for (unsigned int i = 0; i < school_attendants.size(); i++) { // eliminates warning...
                        // for (auto& a_school_attendant : school_attendants) {
                        // choose random households to be assigned to the city
                        trng::uniform_int_dist distr(0, (unsigned int)contact_pools.size());
                        // Raphael@Nishchal, commenting this out to suppress warning,
                        // uncomment whenever you start using 'index'
                        // unsigned int           index = geogen::generator.GetGenerator(distr)();
                        // TODO use stride::Person class
                        // contact_pools.at(index)->AddMember(a_school_attendant);

                        // cout<< a_school_attendant.age << " has been added to contact_pool " << index<<endl;
                }
        }
}

vector<shared_ptr<stride::ContactPool>> PopulationGenerator::GetContactPoolsOfColleges()
{
        vector<shared_ptr<stride::ContactPool>> result;
        for (auto& a_city : m_geogrid.GetCitiesWithCollege()) {
                for (auto& a_comunity : a_city->GetCommunitiesOfType(geogen::CommunityType::College)) {
                        auto contact_pools = a_comunity->GetContactPools();
                        result.insert(result.end(), contact_pools.begin(), contact_pools.end());
                }
        }
        return result;
}

void PopulationGenerator::AssignToColleges()
{
        auto contact_pools = GetContactPoolsOfColleges();

        vector<Person> students;
        for (auto& a_city : m_geogrid.GetCities()) {
                for (auto& hh : a_city.second->GetHouseholds()) {
                        hh->GetCollegeStudents(students);
                }
        }

        for (auto& a_student : students) {
                // it is chosen randomly right now
                // must look at other factors like possiblity to commute
                // rate of studying from home and so
                trng::uniform_int_dist distr(0, (unsigned int)contact_pools.size());
                unsigned int           index = (unsigned int)geogen::generator.GetGenerator(distr)();
                cout << a_student.age << " is added to contact pool " << index << endl;
        }
}

void PopulationGenerator::AssignToWorkplaces() {}

void PopulationGenerator::AssignToCommunity()
{
        for (auto& a_city : m_geogrid.GetCities()) {
                auto contact_pools = GetNearbyContactPools(*(a_city.second), geogen::CommunityType::Primary);
                for (auto& a_household : a_city.second->GetHouseholds()) {
                        for (auto& a_person : a_household->GetMembers()) {
                                // assign the person to a random contactPool
                                trng::uniform_int_dist distr(0, (unsigned int)contact_pools.size());
                                unsigned int           index = geogen::generator.GetGenerator(distr)();

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
