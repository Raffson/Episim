//
// Created by beau on 3/5/18.
//

#include "GeoGrid.h"

using namespace std;

namespace geogen {

void GeoGrid::GetMainFractions(const vector<shared_ptr<Household>>& hhs)
{
        unsigned int schooled = 0;
        unsigned int workers1 = 0;
        unsigned int workers2 = 0;
        unsigned int toddlers = 0;
        unsigned int oldies   = 0;
        for (auto& house : hhs) {
                for (auto& member : house->GetMembers()) {
                        // Ordered these if-else if construction to fall as quickly as possible
                        // in the (statistically) most likely age-category...
                        // it's agueable whether "rest" is more likely than workers1 since it covers
                        // a bigger interval, i.e. [0,3) U [65, +inf)
                        if (member.age >= 26 and member.age < 65)
                                workers2 += 1;
                        else if (member.age >= 3 and member.age < 18)
                                schooled += 1;
                        else if (member.age >= 18 and member.age < 26)
                                workers1 += 1;
                        else if (member.age >= 65)
                                oldies += 1;
                        else
                                toddlers += 1;
                }
        }
        float total     = schooled + workers1 + workers2 + toddlers + oldies;
        m_schooled_frac = schooled / total;
        m_workers1_frac = workers1 / total;
        m_workers2_frac = workers2 / total;
        m_toddlers_frac = toddlers / total;
        m_oldies_frac   = oldies / total;
}

GeoGrid::GeoGrid(const boost::filesystem::path& config_file)
{

        REQUIRE(file_exists(config_file), "Could not find the provided configuration file");

        this->m_school_count = 0;
        // Setting up property tree to parse xml config file
        boost::property_tree::ptree p_tree;
        boost::property_tree::read_xml(config_file.string(), p_tree);

        // reading the cities data file
        string base_path      = "data/";
        string city_file      = p_tree.get("popgen.data_files.cities", "flanders_cities.csv");
        string commuting_file = p_tree.get("popgen.data_files.commuting", "flanders_commuting.csv");
        string household_file = p_tree.get("popgen.data_files.households", "households_flanders.xml");

        m_cities = parser::ParseCities(base_path + city_file, base_path + commuting_file, true);

        households = parser::ParseHouseholds(base_path + household_file);

        // Generating schools
        // auto total_pop = p_tree.get<unsigned int>("popgen.pop_info.pop_total");
        // specs ask this to be read out of config, but could be calculated directly
        // out of the city file?
        // -> you're right... so let's do it like this:
        // m_total_pop = CountTotalPop();
        // After specifically asking about this, turns out we still need to read it from file...
        // perhaps find a way to verify this number somehow, if the possibility exists of course...
        m_total_pop = p_tree.get<unsigned int>("popgen.pop_info.pop_total");

        GetMainFractions(households);

        m_student_frac            = p_tree.get<float>("popgen.pop_info.fraction_students");
        m_commuting_students_frac = p_tree.get<float>("popgen.pop_info.fraction_commuting_students");
        m_active_frac             = p_tree.get<float>("popgen.pop_info.fraction_active_workers");
        m_commuting_workers_frac  = p_tree.get<float>("popgen.pop_info.fraction_commuting_workers");

        m_avg_cp_size     = p_tree.get<unsigned int>("popgen.contactpool_info.average_size");
        m_school_size     = p_tree.get<unsigned int>("popgen.contactpool_info.school.size");
        m_college_size    = p_tree.get<unsigned int>("popgen.contactpool_info.college.size");
        m_maxlc           = p_tree.get<unsigned int>("popgen.contactpool_info.college.cities");
        m_community_size  = p_tree.get<unsigned int>("popgen.contactpool_info.community.size");
        m_worksplace_size = p_tree.get<unsigned int>("popgen.contactpool_info.workplace.size");

        // Setting up RNG
        unsigned long   seed = (unsigned long)(p_tree.get("popgen.rng.seed", 0));
        string type = p_tree.get("popgen.rng.type", "mrg2");
        generator   = stride::util::RNManager(stride::util::RNManager::Info(type, seed));

        // rounding errors cause the first ensure to fail in some conditions...
        // however, is this first ENSURE necessary?
        // it should never fail since we decude the fractions from the households,
        // so removed the correspronding death test until we find a better test...
        float epsilon   = 0.000001;
        float totalfrac = m_workers1_frac + m_workers2_frac + m_toddlers_frac + m_oldies_frac + m_schooled_frac;
        ENSURE(fabs(totalfrac - 1) < epsilon, "Pop frac should equal 1");
        ENSURE(1 >= m_student_frac and m_student_frac >= 0, "Student fraction must be between 0 and 1");
        ENSURE(1 >= m_commuting_students_frac and m_commuting_students_frac >= 0,
               "Student Commuting fraction must be between 0 and 1");
        ENSURE(1 >= m_active_frac and m_active_frac >= 0, "Active workers fraction must be between 0 and 1");
        ENSURE(1 >= m_commuting_workers_frac and m_commuting_workers_frac >= 0,
               "Commuting workers fraction must be between 0 and 1");
        ENSURE(m_avg_cp_size > 0, "Contactpool's size must be bigger than 0");
}

void GeoGrid::GenerateAll()
{
        GenerateSchools();
        GenerateColleges();
        GenerateWorkplaces();
        GenerateCommunities();
}

void GeoGrid::GenerateSchools()
{

        REQUIRE(m_schooled_frac <= 1, "Schooled Fract is bigger then 1, not possible!");
        REQUIRE(m_schooled_frac >= 0, "Schooled fract can't be negative");
        REQUIRE(m_school_size >= 0, "The initial school size can't be negative");
        // Calculating extra data
        // rounded because we don't have a fraction of a person
        auto amount_schooled = (const unsigned int)round(m_total_pop * m_schooled_frac);
        // round because we do not build half a school
        auto amount_of_schools = (const unsigned int)round(amount_schooled / m_school_size);

        // Determine number of contactpools
        auto cps = round(m_school_size / m_avg_cp_size); /// We need enough pools to distribute all persons

        // Setting up to divide the schools to cities
        vector<unsigned int> pop_id; // We will push the id's of the cities for each pop member.
        for (auto& it : m_cities) {
                auto c_schooled_pop = (unsigned int)round(it.second->GetPopulation() * m_schooled_frac);
                pop_id.insert(pop_id.end(), c_schooled_pop, (const unsigned int&)it.first);
        }
        // Note that this way cuz of rounding we lose a couple of schooled ppl.
        // But this shouldn't affect our city divison.
        trng::uniform_int_dist distr(0, (unsigned int)pop_id.size() - 1);

        // assign schools to cities according to our normal distribution
        for (unsigned int i = 0; i < amount_of_schools; i++) {
                m_school_count++;
                int                   index       = pop_id[generator.GetGenerator(distr)()];
                shared_ptr<City>      chosen_city = m_cities[index];
                shared_ptr<Community> nw_school(new Community(CommunityType::School, chosen_city));

                // Add contactpools
                for (auto j = 0; j < cps; j++) {
                        stride::ContactProfiles contactProfiles{};
                        auto                    pool = std::make_shared<stride::ContactPool>(
                            m_id_generator, stride::ContactPoolType::Id::School, contactProfiles);
                        m_id_generator++;
                        nw_school->AddContactPool(pool);
                }

                chosen_city->AddCommunity(nw_school);
                // m_communities[nw_school->getID()] = nw_school
        }
        // We should ENSURE schools are effectively placed in cities.
        // The OO nature makes this assertion rather complex -> found in tests
}

unsigned int GeoGrid::FindSmallest(const vector<shared_ptr<City>>& lc)
{
        unsigned int smallest = 0;
        for (unsigned int i = 1; i < lc.size(); i++) {
                if (lc[smallest]->GetPopulation() > lc[i]->GetPopulation())
                        smallest = i;
        }
        return smallest;
}

void GeoGrid::AdjustLargestCities(vector<shared_ptr<City>>& lc, const shared_ptr<City>& city)
{
        if (lc.size() < m_maxlc)
                lc.push_back(city);
        else {
                unsigned int citpop   = city->GetPopulation();
                unsigned int smallest = FindSmallest(lc);
                if (citpop > lc[smallest]->GetPopulation())
                        lc[smallest] = city;
        }
}

void GeoGrid::GenerateColleges()
{
        // need m_maxlc largest cities, largest determined by number of people in the city...

        // After deducing fractions from households, these should never fail,
        // they also become difficult to test since we can no longer play with the fractions,
        // gotta come up with new tests for this...
        REQUIRE(m_student_frac >= 0, "Student fractal can't be negative");
        REQUIRE(m_student_frac <= 1, "Student fractal can't be more then 100%");
        REQUIRE(m_workers1_frac >= 0, "Worker fractal can't be negative");
        REQUIRE(m_workers1_frac <= 1, "Worker fractal can't be more then 100%");
        for (auto& it : m_cities) {
                AdjustLargestCities(m_cities_with_college, it.second);
        }

        // Determine number of contactpools
        auto cps = round(m_college_size / m_avg_cp_size); /// We need enough pools to distribute all persons

        // generate colleges to the respective cities...
        for (auto& it : m_cities_with_college) {
                double students = it->GetPopulation() * m_workers1_frac * m_student_frac;
                // doesn't matter if students is a double at this time
                // since this is only an estimate for the number of colleges
                auto nrcolleges = (unsigned int)round(students / m_college_size);

                // TODO Is it not given wich cities have exactly one college, calculation needed??
                // -> tricky question, suppose this city has less than (m_college_size*0.5) students
                //      then nrcolleges would round to 0 and we would have a problem...
                //      the result would be that m_maxlc will be bigger than the number of cities with colleges...
                //      this is a valid remark...

                for (unsigned int i = 0; i < nrcolleges; i++) {
                        shared_ptr<Community> college = make_shared<Community>(CommunityType::College, it);
                        // Add contactpools
                        for (auto j = 0; j < cps; j++) {
                                stride::ContactProfiles contactProfiles{};
                                auto                    pool = std::make_shared<stride::ContactPool>(
                                    m_id_generator, stride::ContactPoolType::Id::School, contactProfiles);
                                m_id_generator++;
                                college->AddContactPool(pool);
                        }
                        it->AddCommunity(college);
                        // m_communities[college->getID()] = college
                }
        }
}

void GeoGrid::GenerateWorkplaces()
{
        // TODO This should also be placed according to a discrete distribution!
        // TODO still figuring out how i should do this for this function
        // is this being done by now?

        for (auto it : m_cities) {
                shared_ptr<City> city         = it.second;
                unsigned int     in_commuters = city->GetTotalInCommutersCount();

                // some percentages of the commuters are students
                double working_commuters    = m_commuting_workers_frac * (in_commuters);
                auto   number_of_workplaces = (unsigned int)round(working_commuters / m_worksplace_size);

                for (unsigned int i = 0; i < number_of_workplaces; i++) {
                        shared_ptr<Community> community = make_shared<Community>(CommunityType::Work, city);
                        /// Add contactpools
                        stride::ContactProfiles contactProfiles{};
                        auto                    pool = std::make_shared<stride::ContactPool>(
                            m_id_generator, stride::ContactPoolType::Id::Work, contactProfiles);
                        m_id_generator++;
                        community->AddContactPool(pool);
                        city->AddCommunity(community);
                }
        }
}

// Communities need to be distributed according to the relative population size.
// TODO On average a community has 2000 -> Should not be hardcorded members.
//  -> recall that 2000 was in fact a hard limit...
//      we need to ask (tuesday) what happened after the professor's refractoring,
//      i.e. if the hard limit still holds...
void GeoGrid::GenerateCommunities()
{

        // Determine number of contactpools
        auto cps = round(m_community_size / m_avg_cp_size); /// We need enough pools to distribute all persons

        vector<shared_ptr<Community>> primsec_communities;
        // First we need to determine the total number of communities to be used.
        auto total_communities = (unsigned int)ceil(m_total_pop / m_community_size);

        vector<unsigned int> pop_id;
        for (auto& it : m_cities) {
                auto c_schooled_pop = (unsigned int)round(it.second->GetPopulation() * m_schooled_frac);
                pop_id.insert(pop_id.end(), c_schooled_pop, (const unsigned int&)it.first);
        }

        // Note that this way cuz of rounding we lose a couple of schooled ppl.
        // But this shouldn't affect our city divison.
        trng::uniform_int_dist distr(0, (unsigned int)pop_id.size() - 1);

        for (unsigned int i = 0; i < total_communities; i++) {
                m_school_count++;
                int                   index       = pop_id[generator.GetGenerator(distr)()];
                shared_ptr<City>      chosen_city = m_cities[index];
                shared_ptr<Community> nw_community(new Community(CommunityType::Primary, chosen_city));
                // Add contactpools
                for (auto j = 0; j < cps; j++) {
                        stride::ContactProfiles contactProfiles{};
                        auto                    pool = std::make_shared<stride::ContactPool>(
                            m_id_generator, stride::ContactPoolType::Id::PrimaryCommunity, contactProfiles);
                        m_id_generator++;
                        nw_community->AddContactPool(pool);
                }
                chosen_city->AddCommunity(nw_community);
        }
}

unsigned int GeoGrid::CountTotalPop() const
{

        unsigned int counter = 0;
        for (auto& it : m_cities) {
                counter += it.second->GetPopulation();
        }
        return counter;
}

const map<int, shared_ptr<City>>& GeoGrid::GetCities() { return m_cities; }

unsigned int GeoGrid::GetSchoolCount() const { return m_school_count; }

shared_ptr<City>& GeoGrid::operator[](int i) { return m_cities[i]; }

Coordinate GeoGrid::GetCenterOfGrid()
{
        double smallestX    = numeric_limits<double>::max();
        double biggestX     = numeric_limits<double>::lowest();
        double smallestY    = numeric_limits<double>::max();
        double biggestY     = numeric_limits<double>::lowest();
        double smallestLat  = 180.0;
        double biggestLat   = -180.0;
        double smallestLong = 180.0;
        double biggestLong  = -180.0;

        // Raphael@everyone, excellent example of something we could run in parallel!!!
        // don't understand me wrong, not right now, but later when we're going for beta or in the final phase...
        for (auto& city : m_cities) {
                Coordinate cc = city.second->GetCoordinates();
                if (cc.x < smallestX)
                        smallestX = cc.x;
                if (cc.x > biggestX)
                        biggestX = cc.x;
                if (cc.y < smallestY)
                        smallestY = cc.y;
                if (cc.y > biggestY)
                        biggestY = cc.y;
                if (cc.latitude < smallestLat)
                        smallestLat = cc.latitude;
                if (cc.latitude > biggestLat)
                        biggestLat = cc.latitude;
                if (cc.longitude < smallestLong)
                        smallestLong = cc.longitude;
                if (cc.longitude > biggestLong)
                        biggestLong = cc.longitude;
        }

        double halfX    = (biggestX - smallestX) / 2;
        double halfY    = (biggestY - smallestY) / 2;
        double halfLat  = (biggestLat - smallestLat) / 2;
        double halfLong = (biggestLong - smallestLong) / 2;

        return Coordinate((smallestX + halfX), (smallestY + halfY), (smallestLong + halfLong), (smallestLat + halfLat));
}

} // namespace geogen
