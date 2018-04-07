//
// Created by beau on 3/5/18.
//

#include "GeoGrid.h"
#include "trng/discrete_dist.hpp"
using namespace std;

namespace geogen {

void GeoGrid::GetMainFractions(const vector<vector<double>>& hhs)
{
        unsigned int schooled = 0;
        unsigned int workers1 = 0;
        unsigned int workers2 = 0;
        unsigned int toddlers = 0;
        unsigned int oldies   = 0;
        for (auto& house : hhs) {
                for (auto& age : house) {
                        // Ordered these if-else if construction to fall as quickly as possible
                        // in the (statistically) most likely age-category...
                        // it's agueable whether "rest" is more likely than workers1 since it covers
                        // a bigger interval, i.e. [0,3) U [65, +inf)
                        if (age >= 26 and age < 65)
                                workers2 += 1;
                        else if (age >= 3 and age < 18)
                                schooled += 1;
                        else if (age >= 18 and age < 26)
                                workers1 += 1;
                        else if (age >= 65)
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

        // Raphael@everyone, until the refractor occurs, I can't delete/rename this..
        // however i want to start making my functions with the correct signatures for after the refractor,
        // therefore I will rename this member to a more appropriate name for after the refractor,
        // as a result, we'll just need to delete some stuff out of the header file and that'll be it...
        m_household_age_distr = parser::ParseHouseholds(base_path + household_file);

        // Generating schools
        // auto total_pop = p_tree.get<unsigned int>("popgen.pop_info.pop_total");
        // specs ask this to be read out of config, but could be calculated directly
        // out of the city file?
        // -> you're right... so let's do it like this:
        // m_total_pop = CountTotalPop();
        // After specifically asking about this, turns out we still need to read it from file...
        // perhaps find a way to verify this number somehow, if the possibility exists of course...
        m_total_pop = p_tree.get<unsigned int>("popgen.pop_info.pop_total");

        GetMainFractions(m_household_age_distr);

        m_student_frac            = abs(p_tree.get<float>("popgen.pop_info.fraction_students"));
        m_commuting_students_frac = abs(p_tree.get<float>("popgen.pop_info.fraction_commuting_students"));
        m_active_frac             = abs(p_tree.get<float>("popgen.pop_info.fraction_active_workers"));
        m_commuting_workers_frac  = abs(p_tree.get<float>("popgen.pop_info.fraction_commuting_workers"));

        m_avg_cp_size     = (unsigned int) abs(p_tree.get<long>("popgen.contactpool_info.average_size"));
        m_school_size     = (unsigned int) abs(p_tree.get<long>("popgen.contactpool_info.school.size"));
        m_college_size    = (unsigned int) abs(p_tree.get<long>("popgen.contactpool_info.college.size"));
        m_maxlc           = (unsigned int) abs(p_tree.get<long>("popgen.contactpool_info.college.cities"));
        m_community_size  = (unsigned int) abs(p_tree.get<long>("popgen.contactpool_info.community.size"));
        m_worksplace_size = (unsigned int) abs(p_tree.get<long>("popgen.contactpool_info.workplace.size"));

        // Setting up RNG
        unsigned long seed = (unsigned long) abs(p_tree.get("popgen.rng.seed", 0));
        string type        = p_tree.get("popgen.rng.type", "mrg2");
        generator.Initialize(stride::util::RNManager::Info(type, seed));
        //TODO: ^ somewhere else. Wait untill we integrate with stride

        // rounding errors cause the first ensure to fail in some conditions...
        // however, is this first ENSURE necessary?
        // it should never fail since we decude the fractions from the households,
        // so removed the correspronding death test until we find a better test...
        float totalfrac = m_workers1_frac + m_workers2_frac + m_toddlers_frac + m_oldies_frac + m_schooled_frac;
        ENSURE(fabs(totalfrac - 1) < constants::EPSILON, "Pop frac should equal 1");
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
        vector<unsigned int> p_vec;
        vector<shared_ptr<City>> c_vec;
        for (auto& it : m_cities) {
                auto c_schooled_pop = (unsigned int)round(it.second->GetPopulation() * m_schooled_frac);
                p_vec.push_back(c_schooled_pop);
                c_vec.push_back(it.second);
        }
        // Note that this way cuz of rounding we lose a couple of schooled ppl.
        // But this shouldn't affect our city divison.
        trng::discrete_dist distr(p_vec.begin(), p_vec.end());

        // assign schools to cities according to our normal distribution
        for (unsigned int i = 0; i < amount_of_schools; i++) {
                m_school_count++;
                shared_ptr<City>      chosen_city = c_vec[generator.GetGenerator(distr)()];
                shared_ptr<Community> nw_school(new Community(CommunityType::School, chosen_city));

                // Add contactpools
                for (auto j = 0; j < cps; j++) {
                        auto                    pool = std::make_shared<stride::ContactPool>(
                            m_id_generator, stride::ContactPoolType::Id::School);
                        m_id_generator++;
                        nw_school->AddContactPool(pool);
                }

                chosen_city->AddCommunity(nw_school);
                // m_communities[nw_school->getID()] = nw_school TODO: What is this??
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

                for (unsigned int i = 0; i < nrcolleges; i++) {
                        shared_ptr<Community> college = make_shared<Community>(CommunityType::College, it);
                        // Add contactpools
                        for (auto j = 0; j < cps; j++) {
                                auto                    pool = std::make_shared<stride::ContactPool>(
                                    m_id_generator, stride::ContactPoolType::Id::School);
                                m_id_generator++;
                                college->AddContactPool(pool);
                        }
                        it->AddCommunity(college);
                        // m_communities[college->getID()] = college
                }
        }
}


void GeoGrid::GenerateWorkplaces(){
    //We create the vec we will choose our city_id's out of.
    //Meaning a citiy has a proballity to get assigned a workplace equal to the fraction
    //of people working IN the city (not the active working pop in the city).
    //We have to account for the commuters in he city.

    vector<double> lottery_vec; // vector of relative probabillitys
    vector<shared_ptr<City>> c_vec;// we will use this to vec to map the city to a set of sequential numbers 0...n
    for(auto& it: m_cities){

        // This also calculates people living and working in the same city
        auto work_pop = (unsigned int) round(it.second->GetTotalInCommutersCount() * m_commuting_workers_frac);
        // Inserting the amount of id's of the city equal to the pop working in the city
        lottery_vec.push_back(work_pop);
        c_vec.push_back(it.second);
    }

    // Now we calculate how many workplaces we have to create.
    double working_commuters = m_commuting_workers_frac * m_total_pop;
    auto   number_of_workplaces = (unsigned int)round(working_commuters / m_worksplace_size);

    trng::discrete_dist distr(lottery_vec.begin(), lottery_vec.end()); //Setting up distribution

    //Now we will place each workplace randomly in our city, making use of our lottery vec.
    for(unsigned int i = 0;  i < number_of_workplaces; i++){

        shared_ptr<City>      chosen_city = c_vec[generator.GetGenerator(distr)()];
        shared_ptr<Community> nw_workplace(new Community(CommunityType::Work, chosen_city));

        // A workplace has a contactpool.
        auto pool = std::make_shared<stride::ContactPool>(m_id_generator, stride::ContactPoolType::Id::Work);
        m_id_generator++;
        nw_workplace->AddContactPool(pool);
        chosen_city->AddCommunity(nw_workplace);
    }

}

/* Wrong code -> Depricated
void GeoGrid::GenerateWorkplaces()
{
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
                        auto pool = std::make_shared<stride::ContactPool>(
                                    m_id_generator, stride::ContactPoolType::Id::Work);
                        m_id_generator++;
                        community->AddContactPool(pool);
                        city->AddCommunity(community);
                }
        }
}*/

// Communities need to be distributed according to the relative population size.
void GeoGrid::GenerateCommunities()
{

        // Determine number of contactpools
        auto cps = round(m_community_size / m_avg_cp_size); /// We need enough pools to distribute all persons

        vector<shared_ptr<Community>> primsec_communities;
        // First we need to determine the total number of communities to be used.
        auto total_communities = (unsigned int)ceil(m_total_pop / m_community_size);

        vector<unsigned int> p_vec;
        vector<shared_ptr<City>>c_vec;
        for (auto& it : m_cities) {
                auto c_schooled_pop = (unsigned int)round(it.second->GetPopulation() * m_schooled_frac);
                p_vec.push_back(c_schooled_pop);
                c_vec.push_back(it.second);
        }

        // Note that this way cuz of rounding we lose a couple of schooled ppl.
        // But this shouldn't affect our city divison.
        trng::discrete_dist distr(p_vec.begin(), p_vec.end());

        for (unsigned int i = 0; i < total_communities; i++) {
                m_school_count++;
                shared_ptr<City>      chosen_city = c_vec[generator.GetGenerator(distr)()]
                shared_ptr<Community> nw_community(new Community(CommunityType::Primary, chosen_city));
                // Add contactpools
                for (auto j = 0; j < cps; j++) {
                        auto                    pool = std::make_shared<stride::ContactPool>(
                            m_id_generator, stride::ContactPoolType::Id::PrimaryCommunity);
                        m_id_generator++;
                        nw_community->AddContactPool(pool);
                }
                chosen_city->AddCommunity(nw_community);
        }
        // Determine how many communities a city should get -> Depricated.
        /*for (auto it : m_cities){
            shared_ptr<City> city = it.second;
            // ratio: the city contains ratio % of the total population.
            double ratio = (city->GetPopulation()/(double)m_total_pop);
            assert(0<ratio<1);
            // Now we have the ratio, we know that the city has ratio % of all communities.
            auto city_communities = (total_communities*ratio);
            assert(city_communities<total_communities);

            for (int i = 0; i < city_communities; i++){
                /// Since there currently is no real difference between primary and secundary communities we make them
        all primary. shared_ptr<Community> community = make_shared<Community>(CommunityType::Primary, city);
                city->AddCommunity(community);
                //m_communities[community->getID()] = community
            }
        }*/
}

/*unsigned int GeoGrid::CountTotalPop() const
{

        unsigned int counter = 0;
        for (auto& it : m_cities) {
                counter += it.second->GetPopulation();
        }
        return counter;
}*/

const map<int, shared_ptr<City>>& GeoGrid::GetCities() { return m_cities; }

unsigned int GeoGrid::GetSchoolCount() const { return m_school_count; }

shared_ptr<City>& GeoGrid::operator[](int i) { return m_cities[i]; }

Coordinate GeoGrid::GetCenterOfGrid()
{
        double smallestX    = numeric_limits<double>::max();
        double biggestX     = numeric_limits<double>::lowest();
        double smallestY    = numeric_limits<double>::max();
        double biggestY     = numeric_limits<double>::lowest();
        double smallestLat  = 180.0; // Highest lat possible (initial)
        double biggestLat   = -180.0; // Lowest lat  possible (initial)
        double smallestLong = 180.0; // highest long possible (initial)
        double biggestLong  = -180.0; // lowest long possible (initial)

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

        return {(smallestX + halfX), (smallestY + halfY), (smallestLong + halfLong),
                (smallestLat + halfLat)};
}

} // namespace geogen
