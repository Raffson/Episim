//
// Created by Nishchal Shrestha on 20/03/2018.
//

#include "PopulationGenerator.h"

namespace popgen {

PopulationGenerator::PopulationGenerator(geogen::GeoGrid geogrid)
    : m_geogrid(geogrid)
{
}

void PopulationGenerator::AssignHouseholds()
{

        // households should be assigned to cities
        vector<Household> households = m_geogrid.GetModelHouseholds();

        for (auto& a_city : m_geogrid.GetCities()) {
                const unsigned int max_population       = a_city.second->GetPopulation();
                int                remaining_population = (int)max_population;

                while (remaining_population > 0) {
                        // choose random households to be assigned to the city
                        trng::uniform_int_dist distr(0, (unsigned int)households.size() - 1);
                        unsigned int           index = geogen::generator.GetGenerator(distr)();

                        households.at(index).SetCityID(a_city.second->GetId());
                        auto a_household = std::make_shared<Household>();

                        for (auto a_member : households.at(index).GetMembers()) {
                                a_household->AddMember(a_member);
                        }

                        a_city.second->AddHousehold(a_household);
                        remaining_population -= households.at(index).GetSize();

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

// Raphael@Nishchal this is good shit, however on the scale that we're looking
// I wonder if it makes sense to go this far since we have euclidian coordinates as well...
// nevertheless, this is still good shit and on larger scales this will be essential...
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
        // Raphael@Nishchal this hardcoded value of 20 is a NO-GO!
        // we'll need to discuss how to handle this properly...
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
                unsigned int                            radius = 10;
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
                                                for (unsigned int i = 0; i < avg_contactpools_per_school; i++) {
                                                        std::shared_ptr<stride::ContactPool> pool;
                                                        a_community->AddContactPool(pool);
                                                        contact_pools.push_back(pool);
                                                }
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
                for (auto& a_school_attendant : school_attendants) {
                        // choose random households to be assigned to the city
                        trng::uniform_int_dist distr(0, (unsigned int)contact_pools.size() - 1);
                        unsigned int           index = geogen::generator.GetGenerator(distr)();
                        // TODO use stride::Person class
                        // contact_pools.at(index)->AddMember(a_school_attendant);

                        // cout<< a_school_attendant.age << " has been added to contact_pool " << index<<endl;
                }
        }
}

void PopulationGenerator::AssignToColleges() {}

void PopulationGenerator::AssignToWorkplaces() {}

void PopulationGenerator::AssignToCommunity() {}

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
