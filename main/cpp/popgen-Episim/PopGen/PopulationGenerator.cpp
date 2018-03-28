//
// Created by Nishchal Shrestha on 20/03/2018.
//

#include "PopulationGenerator.h"

namespace popgen {

PopulationGenerator::PopulationGenerator(geogen::GeoGrid geogrid, unsigned int rad)
    : m_geogrid(geogrid), m_initial_search_radius(rad)
{
}

void PopulationGenerator::AssignHouseholds()
{

        // households should be assigned to cities
        vector<shared_ptr<Household>> households = m_geogrid.GetModelHouseholds();

        for (auto& a_city : m_geogrid.GetCities()) {
                const unsigned int max_population       = a_city.second->GetPopulation();
                int                remaining_population = (int)max_population;

                while (remaining_population > 0) {
                        // choose random households to be assigned to the city
                        trng::uniform_int_dist distr(0, (unsigned int)households.size() - 1);
                        unsigned int           index = geogen::generator.GetGenerator(distr)();

                        households.at(index)->SetCityID(a_city.second->GetId());
                        auto a_household = std::make_shared<Household>();

                        for (auto& a_member : households.at(index)->GetMembers()) {

                                if (a_member.age >= 18 && a_member.age < 26) {
                                        // 50% probability that he works and 50% that he goes to college
                                        trng::uniform_int_dist distr2(0, 2);
                                        if (geogen::generator.GetGenerator(distr2)()) {
                                                // a_member.work_id = std::numeric_limits<unsigned int>::infinity();
                                                a_member.work_id = 9999; // TODO must make this infinite
                                        }
                                }
                                a_household->AddMember(a_member);
                        }

                        a_city.second->AddHousehold(a_household);
                        remaining_population -= households.at(index)->GetSize();

                        for (auto a_member : households.at(index)->GetMembers()) {
                                a_household->AddMember(a_member);
                        }

                        // This can be uncommented to see which city has what kind of households
                        // cout <<"Household = " << households.at(index).GetID() << " is added to " <<
                        // a_city.second->GetName()<< endl;
                }
        }
}

std::vector<std::shared_ptr<geogen::City>> PopulationGenerator::GetCitiesWithinRadius(geogen::City origin,
                                                                                      unsigned int radius)
{
        // TODO to save time we can add distances between two cities in a map but will cost some space
        std::vector<std::shared_ptr<geogen::City>> result;
        for (auto a_city : m_geogrid.GetCities()) {
                double distance = GetDistance(a_city.second->GetCoordinates(), origin.GetCoordinates());
                if (distance <= radius) {
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

void PopulationGenerator::AssignToSchools()
{
        // TODO Shall we add this information in geogrid?
        // Raphael@everyone, this hard limit should be part of one of the config files...
        const unsigned int avg_students_in_contactpool = 20;
        const unsigned int avg_contactpools_per_school = m_geogrid.GetSchoolSize() / avg_students_in_contactpool;

        // Collecting all the school attendants from the city
        for (auto& a_city : m_geogrid.GetCities()) {
                vector<Person> school_attendants;
                for (auto& a_household : a_city.second->GetHouseholds()) {
                        vector<Person> current_school_attendants;
                        a_household->GetSchoolAttendants(current_school_attendants);
                        for (auto a_school_attendant : current_school_attendants) {
                                school_attendants.push_back(a_school_attendant);
                        }
                }

                // Search schools within 10km radius otherwise double the radius untill we find schools
                // Raphael@Nishchal again, this hardcoded value is ugly... gotta find a better solution...
                unsigned int                            radius = m_initial_search_radius;
                vector<shared_ptr<geogen::Community>>   near_schools;
                map<int, shared_ptr<geogen::City>>      all_cities = m_geogrid.GetCities();
                vector<shared_ptr<stride::ContactPool>> contact_pools;

                while (true) {
                        std::vector<std::shared_ptr<geogen::City>> near_cities =
                            GetCitiesWithinRadius(*(a_city.second), radius);

                        // fetching schools
                        for (auto& near_city : near_cities) {
                                for (auto& a_community : near_city->GetAllCommunities()) {
                                        if (a_community->GetCommunityType() == geogen::CommunityType::School) {
//                                                for (unsigned int i = 0; i < avg_contactpools_per_school; i++) {
//                                                        std::shared_ptr<stride::ContactPool> pool;
//                                                        a_community->AddContactPool(pool);
//                                                        contact_pools.push_back(pool);
//                                                }
                                                near_schools.push_back(a_community);
                                        }
                                }
                        }

                        if (near_schools.size() == 0) {
                                radius *= 2;
                        } else {
                                break;
                        }
                } // end while

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

void PopulationGenerator::AssignToColleges()
{
        vector<Person*> college_students_commuters;
        vector<Person*> college_students_home;

        const unsigned int avg_people_in_contactpool = 20;
        const unsigned int avg_contact_pools         = m_geogrid.GetCollegeSize() / avg_people_in_contactpool;

        // creating contactpools
//        for (auto& a_city : m_geogrid.GetCitiesWithCollege()) {
//                for (auto& a_community : a_city->GetAllCommunities()) {
//                        if (a_community->GetCommunityType() == geogen::CommunityType::College) {
//                                for (unsigned int i = 0; i < avg_contact_pools; i++) {
//                                        stride::ContactProfiles contactProfiles;
//                                        auto                    pool = std::make_shared<stride::ContactPool>(
//                                            m_id_generator, stride::ContactPoolType::Id::School, contactProfiles);
//                                        m_id_generator++;
//                                        a_community->AddContactPool(pool);
//                                }
//                        }
//                }
//        }

        for (auto& a_city : m_geogrid.GetCities()) {
                for (auto& hh : a_city.second->GetHouseholds()) {
                        for (auto& a_person : hh->GetMembers()) {
                                if (a_person.work_id == 0 && a_person.age >= 18 && a_person.age < 26) {

                                        // 50% studying nearby home
                                        trng::uniform_int_dist distr(0, 2);
                                        if (geogen::generator.GetGenerator(distr)()) {
                                                vector<shared_ptr<geogen::City>> nearest_cities;
                                                unsigned int search_radius = m_initial_search_radius;

                                                while (true) {
                                                        for (auto& a_city_with_college :
                                                             m_geogrid.GetCitiesWithCollege()) {
                                                                double distance =
                                                                    GetDistance(a_city.second->GetCoordinates(),
                                                                                a_city_with_college->GetCoordinates());
                                                                if (distance <= search_radius) {
                                                                        nearest_cities.push_back(a_city_with_college);
                                                                }
                                                        }
                                                        if (nearest_cities.size() > 0) {
                                                                break;
                                                        } else {
                                                                search_radius *= 2;
                                                        }
                                                }
                                                cout << "Stuck?" << endl;
                                                vector<shared_ptr<stride::ContactPool>> selected_contact_pools;
                                                // contact_pools.at(index)
//                                                for (auto& a_city : nearest_cities) {
//                                                        for (auto& a_community : a_city->GetAllCommunities()) {
//                                                                if (a_community->GetCommunityType() ==
//                                                                    geogen::CommunityType::College) {
//                                                                        for (auto& a_contact_pool :
//                                                                             a_community->GetContactPools()) {
//                                                                                selected_contact_pools.push_back(
//                                                                                    a_contact_pool);
//                                                                        }
//                                                                }
//                                                        }
//                                                }

                                                trng::uniform_int_dist distr2(0, selected_contact_pools.size());
                                                // Raphael@Nishchal, again suppressing warnings...
                                                // unsigned int index = geogen::generator.GetGenerator(distr2)();

                                                // selected_contact_pools.at(index)->AddMember(&(a_person));

                                        }

                                        // Those who commutes
                                        else {
                                                college_students_commuters.push_back(&a_person);
                                        }
                                }
                        }
                }
        }
        /*
            for(auto& a_city:m_geogrid.GetCitiesWithCollege()){

            }
        */
}

void PopulationGenerator::AssignToWorkplaces() {}

void PopulationGenerator::AssignToCommunity()
{
        for (auto& a_city : m_geogrid.GetCities()) {
                // TODO must read these values somewhere better
                int                search_radius             = m_initial_search_radius;
                const unsigned int avg_people_in_contactpool = 20;
                const unsigned int avg_contactpools_per_community =
                    m_geogrid.GetCommunitySize() / avg_people_in_contactpool;
                // const int maximum_person_in_a_community = 2000;

                vector<shared_ptr<geogen::Community>>   nearest_communities;
                vector<shared_ptr<stride::ContactPool>> nearest_contact_pools;

                while (true) {
                        vector<shared_ptr<geogen::City>> nearest_cities =
                            GetCitiesWithinRadius(*(a_city.second), search_radius);
                        for (auto& a_nearest_city : nearest_cities) {
                                for (auto& a_community : a_nearest_city->GetAllCommunities()) {
                                        if (a_community->GetCommunityType() == geogen::CommunityType::Primary ||
                                            a_community->GetCommunityType() == geogen::CommunityType::Secondary) {

//                                                for (unsigned int i = 0; i < avg_contactpools_per_community; i++) {
//                                                        // Nishchal@everyone Isn't it better idea to create contactpools
//                                                        // while creating all communities
//                                                        stride::ContactProfiles contactProfiles;
//                                                        auto pool = std::make_shared<stride::ContactPool>(
//                                                            m_id_generator, stride::ContactPoolType::Id::School,
//                                                            contactProfiles);
//                                                        m_id_generator++;
//                                                        a_community->AddContactPool(pool);
//                                                        nearest_contact_pools.push_back(pool);
//                                                }
                                                nearest_communities.push_back(a_community);
                                        }
                                }
                        }

                        // no communities nearby double the radius
                        if (nearest_communities.size() == 0) {
                                search_radius *= 2;
                        } else {
                                break;
                        }
                }

                for (auto& a_household : a_city.second->GetHouseholds()) {
                        // Raphael@Nishchal, you know the drill by now...
                        for (unsigned int i = 0; i < a_household->GetMembers().size(); i++) {
                                // for(auto& a_person: a_household->GetMembers()){
                                // assign the person to a random contactPool
                                trng::uniform_int_dist distr(0, (unsigned int)nearest_contact_pools.size());
                                // Raphael@Nishchal, again suppressing warning...
                                // unsigned int index = geogen::generator.GetGenerator(distr)();

                                // TODO the member has to be a stride::Person to be added to stride::ContactPool

                                // nearest_contact_pools.at(index)->AddMember(a_person);
                                // cout <<"A person of age "<< a_person.age << " is added to contact pool at index " <<
                                // index <<endl;

                                // TODO check the community for that contactpool
                                // TODO if the limit is crossed remove from the list of the possible
                                // communities(contactpools)
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
