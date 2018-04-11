//
// Created by beau on 3/5/18.
//

#include "GeoGrid.h"
#include <cstdio>

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
        double total     = schooled + workers1 + workers2 + toddlers + oldies;
        m_fract_map[SCHOOLED] = schooled / total;
        m_fract_map[YOUNG_WORKERS] = workers1 / total;
        m_fract_map[OLD_WORKERS] = workers2 / total;
        m_fract_map[TODDLERS] = toddlers / total;
        m_fract_map[OLDIES]   = oldies / total;
}

void GeoGrid::GetAgeFractions(vector<double>& popfracs)
{
    popfracs.push_back(m_fract_map[SCHOOLED]);
    popfracs.push_back(m_fract_map[YOUNG_WORKERS]);
    popfracs.push_back(m_fract_map[OLD_WORKERS]);
    popfracs.push_back(m_fract_map[TODDLERS]);
    popfracs.push_back(m_fract_map[OLDIES]);
}

GeoGrid::GeoGrid()
{
        m_fract_map[SCHOOLED] = 0;
        m_fract_map[ACTIVE] = 0;
        m_fract_map[YOUNG_WORKERS] = 0;
        m_fract_map[OLD_WORKERS] = 0;
        m_fract_map[TODDLERS] = 0;
        m_fract_map[OLDIES] = 0;
        m_fract_map[STUDENTS] = 0;
        m_fract_map[COMMUTING_STUDENTS] = 0;
        m_fract_map[COMMUTING_WORKERS] = 0;

        m_sizes_map[SCHOOLS] = 0;
        m_sizes_map[COLLEGES] = 0;
        m_sizes_map[COMMUNITES] = 0;
        m_sizes_map[WORKPLACES] = 0;
        m_sizes_map[AVERAGE_CP] = 0;
        m_sizes_map[MAXLC] = 0;
}

GeoGrid::GeoGrid(const boost::filesystem::path& config_file)
{

        REQUIRE(file_exists(config_file), "Could not find the provided configuration file");

        m_school_count = 0;
        // Setting up property tree to parse xml config file
        boost::property_tree::ptree p_tree;

        boost::property_tree::read_xml(config_file.string(), p_tree);

        // reading the cities data file
        string base_path      = "data/";
        string city_file      = p_tree.get("popgen.data_files.cities", "flanders_cities.csv");
        string commuting_file = p_tree.get("popgen.data_files.commuting", "flanders_commuting.csv");
        string household_file = p_tree.get("popgen.data_files.households", "households_flanders.xml");

        parser::ParseCities(base_path + city_file, base_path + commuting_file, m_cities, true);

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

        m_fract_map[STUDENTS]          = abs(p_tree.get<double>("popgen.pop_info.fraction_students"));
        m_fract_map[COMMUTING_STUDENTS]= abs(p_tree.get<double>("popgen.pop_info.fraction_commuting_students"));
        m_fract_map[ACTIVE]            = abs(p_tree.get<double>("popgen.pop_info.fraction_active_workers"));
        m_fract_map[COMMUTING_WORKERS] = abs(p_tree.get<double>("popgen.pop_info.fraction_commuting_workers"));

        m_sizes_map[AVERAGE_CP]     = (unsigned int) abs(p_tree.get<long>("popgen.contactpool_info.average_size"));
        m_sizes_map[SCHOOLS]     = (unsigned int) abs(p_tree.get<long>("popgen.contactpool_info.school.size"));
        m_sizes_map[COLLEGES]    = (unsigned int) abs(p_tree.get<long>("popgen.contactpool_info.college.size"));
        m_sizes_map[MAXLC]           = (unsigned int) abs(p_tree.get<long>("popgen.contactpool_info.college.cities"));
        m_sizes_map[COMMUNITES]  = (unsigned int) abs(p_tree.get<long>("popgen.contactpool_info.community.size"));
        m_sizes_map[WORKPLACES] = (unsigned int) abs(p_tree.get<long>("popgen.contactpool_info.workplace.size"));

        // Setting up RNG
        unsigned long seed = (unsigned long) abs(p_tree.get("popgen.rng.seed", 0));
        string type        = p_tree.get("popgen.rng.type", "mrg2");
        generator.Initialize(stride::util::RNManager::Info(type, seed));
        //TODO: ^ somewhere else. Wait untill we integrate with stride

        // rounding errors cause the first ensure to fail in some conditions...
        // however, is this first ENSURE necessary?
        // it should never fail since we decude the fractions from the households,
        // so removed the correspronding death test until we find a better test...
        //TODO: Working with DesignByContract still relevant?
        // Raphael@Robbe, of course it is, everywhere where we have these REQUIRES and ENSURES, let them be...
        double totalfrac = m_fract_map[YOUNG_WORKERS] + m_fract_map[OLD_WORKERS] + m_fract_map[TODDLERS] +
                m_fract_map[OLDIES] + m_fract_map[SCHOOLED];
        printf("%.16f", totalfrac);
        ENSURE(fabs(totalfrac - 1) < constants::EPSILON, "Pop frac should equal 1");
        ENSURE(1 >= m_fract_map[STUDENTS] and m_fract_map[STUDENTS] >= 0, "Student fraction must be between 0 and 1");
        ENSURE(1 >= m_fract_map[COMMUTING_STUDENTS] and m_fract_map[COMMUTING_STUDENTS] >= 0,
               "Student Commuting fraction must be between 0 and 1");
        ENSURE(1 >= m_fract_map[ACTIVE] and m_fract_map[ACTIVE] >= 0, "Active workers fraction must be between 0 and 1");
        ENSURE(1 >= m_fract_map[COMMUTING_WORKERS] and m_fract_map[COMMUTING_WORKERS] >= 0,
               "Commuting workers fraction must be between 0 and 1");
        //Capping pool size at 1000, gotta ask the professor what the actual cap should be...
        ENSURE(m_sizes_map[AVERAGE_CP] > 0 and m_sizes_map[AVERAGE_CP] <= 1000,
               "Contactpool's size must be bigger than 0 and smaller than or equal to 1000");
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

        REQUIRE(m_fract_map[SCHOOLED] <= 1, "Schooled Fract is bigger then 1, not possible!");
        REQUIRE(m_fract_map[SCHOOLED] >= 0, "Schooled fract can't be negative");
        REQUIRE(m_sizes_map[SCHOOLS] >= 0, "The initial school size can't be negative");
        // Calculating extra data
        // rounded because we don't have a fraction of a person
        auto amount_schooled = (const unsigned int)round(m_total_pop * m_fract_map[SCHOOLED]);
        // round because we do not build half a school
        auto amount_of_schools = (const unsigned int)round(amount_schooled / m_sizes_map[SCHOOLS]);

        // Determine number of contactpools
        auto cps = round(m_sizes_map[SCHOOLS] / m_sizes_map[AVERAGE_CP]); /// We need enough pools to distribute all persons

        // Setting up to divide the schools to cities
        vector<double> p_vec;
        vector<City*> c_vec;
        for (auto& it : m_cities) {
                auto c_schooled_pop = (unsigned int)round(it.second.GetPopulation() * m_fract_map[SCHOOLED]);
                c_vec.emplace_back(&it.second);
                p_vec.emplace_back(c_schooled_pop);
        }
        // Note that this way cuz of rounding we lose a couple of schooled ppl.
        // But this shouldn't affect our city divison.
        auto rndm_vec = generate_random(p_vec, generator, amount_of_schools);
        // assign schools to cities according to our normal distribution
        for (unsigned int i = 0; i < amount_of_schools; i++) {
                m_school_count++;
                City&      chosen_city = *c_vec[rndm_vec[i]];
                Community& nw_school = chosen_city.AddCommunity(CommunityType::School);

                // Add contactpools
                for (auto j = 0; j < cps; j++)
                        nw_school.AddContactPool(stride::ContactPoolType::Id::School);
                // m_communities[nw_school->getID()] = nw_school TODO: What is this??
        }
        // We should ENSURE schools are effectively placed in cities.
        // The OO nature makes this assertion rather complex -> found in tests
}

unsigned int GeoGrid::FindSmallest(const vector<City*>& lc)
{
        unsigned int smallest = 0;
        for (unsigned int i = 1; i < lc.size(); i++) {
                if (lc[smallest]->GetPopulation() > lc[i]->GetPopulation())
                        smallest = i;
        }
        return smallest;
}

void GeoGrid::AdjustLargestCities(vector<City*>& lc, City& city)
{
        if (lc.size() < m_sizes_map[MAXLC])
                lc.emplace_back(&city);
        else {
                unsigned int citpop   = city.GetPopulation();
                unsigned int smallest = FindSmallest(lc);
                if (citpop > lc[smallest]->GetPopulation())
                        lc[smallest] = &city;
        }
}

void GeoGrid::GenerateColleges()
{
        // need m_maxlc largest cities, largest determined by number of people in the city...
        // After deducing fractions from households, these should never fail,
        // they also become difficult to test since we can no longer play with the fractions,
        // gotta come up with new tests for this...
        REQUIRE(m_fract_map[STUDENTS] >= 0, "Student fractal can't be negative");
        REQUIRE(m_fract_map[STUDENTS] <= 1, "Student fractal can't be more then 100%");
        REQUIRE(m_fract_map[YOUNG_WORKERS] >= 0, "Worker fractal can't be negative");
        REQUIRE(m_fract_map[YOUNG_WORKERS] <= 1, "Worker fractal can't be more then 100%");
        for (auto& it : m_cities) {
                AdjustLargestCities(m_cities_with_college, it.second);
        }

        // Determine number of contactpools
        auto cps = round(m_sizes_map[COLLEGES] / m_sizes_map[AVERAGE_CP]); /// We need enough pools to distribute all persons

        // generate colleges to the respective cities...
        for (auto& it : m_cities_with_college) {
                //TODO why is this multiplied with WORKERS 1?
                double students = it->GetPopulation() * m_fract_map[YOUNG_WORKERS] * m_fract_map[STUDENTS];
                // doesn't matter if students is a double at this time
                // since this is only an estimate for the number of colleges
                auto nrcolleges = (unsigned int)round(students / m_sizes_map[COLLEGES]);

                for (unsigned int i = 0; i < nrcolleges; i++) {
                        Community& college = it->AddCommunity(CommunityType::College);
                        // Add contactpools
                        for (auto j = 0; j < cps; j++)
                                college.AddContactPool(stride::ContactPoolType::Id::School);
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
    vector<City*> c_vec;// we will use this to vec to map the city to a set of sequential numbers 0...n
    for(auto& it: m_cities){

        // This also calculates people living and working in the same city
        auto work_pop = (unsigned int) round(it.second.GetTotalInCommutersCount() * m_fract_map[COMMUTING_WORKERS]);
        // Inserting the amount of id's of the city equal to the pop working in the city
        c_vec.emplace_back(&it.second);
        lottery_vec.emplace_back(work_pop);
    }

    // Now we calculate how many workplaces we have to create.
    double working_commuters = m_fract_map[COMMUTING_WORKERS] * m_total_pop;
    auto   number_of_workplaces = (unsigned int)round(working_commuters / m_sizes_map[WORKPLACES]);

    auto rndm_vec = generate_random(lottery_vec, generator, number_of_workplaces);

    //Now we will place each workplace randomly in our city, making use of our lottery vec.
    for(unsigned int i = 0;  i < number_of_workplaces; i++){

        City&      chosen_city = *c_vec[rndm_vec[i]];
        Community& nw_workplace = chosen_city.AddCommunity(CommunityType::Work);

        // A workplace has a contactpool.
        nw_workplace.AddContactPool(stride::ContactPoolType::Id::Work);
    }

}

/* Wrong code -> Depricated
void GeoGrid::GenerateWorkplaces()
{
        // is this being done by now?

        for (auto it : m_cities) {
                City&            city         = it.second;
                unsigned int     in_commuters = city.GetTotalInCommutersCount();

                // some percentages of the commuters are students
                double working_commuters    = m_commuting_workers_frac * (in_commuters);
                auto   number_of_workplaces = (unsigned int)round(working_commuters / m_worksplace_size);

                for (unsigned int i = 0; i < number_of_workplaces; i++) {
                        Community& community = city.AddCommunity(CommunityType::Work, &city);
                        /// Add contactpools
                        community->AddContactPool(stride::ContactPoolType::Id::Work);
                }
        }
}*/

// Communities need to be distributed according to the relative population size.
void GeoGrid::GenerateCommunities()
{

        // Determine number of contactpools
        auto cps = round(m_sizes_map[COMMUNITES] / m_sizes_map[AVERAGE_CP]); /// We need enough pools to distribute all persons

        // First we need to determine the total number of communities to be used.
        auto total_communities = (unsigned int)ceil(m_total_pop / m_sizes_map[COMMUNITES]);

        vector<double> p_vec;
        vector<City*>c_vec;
        for (auto& it : m_cities) {
                auto c_schooled_pop = (unsigned int)round(it.second.GetPopulation() * m_fract_map[SCHOOLED]);
                c_vec.emplace_back(&it.second);
                p_vec.emplace_back(c_schooled_pop);
        }

        // Note that this way cuz of rounding we lose a couple of schooled ppl.
        // But this shouldn't affect our city divison.
        auto rndm_vec = generate_random(p_vec, generator, total_communities);

        for (unsigned int i = 0; i < total_communities; i++) {
                m_school_count++;
                City&      chosen_city = *c_vec[rndm_vec[i]];
                Community& nw_community = chosen_city.AddCommunity(CommunityType::Primary);
                // Add contactpools
                for (auto j = 0; j < cps; j++) {
                        nw_community.AddContactPool(stride::ContactPoolType::Id::PrimaryCommunity);
                }
        }
        // Determine how many communities a city should get -> Depricated.
        /*for (auto it : m_cities){
            City& city = it.second;
            // ratio: the city contains ratio % of the total population.
            double ratio = (city->GetPopulation()/(double)m_total_pop);
            assert(0<ratio<1);
            // Now we have the ratio, we know that the city has ratio % of all communities.
            auto city_communities = (total_communities*ratio);
            assert(city_communities<total_communities);

            for (int i = 0; i < city_communities; i++){
                /// Since there currently is no real difference between primary and secundary communities we make them
        all primary. Community& community = city.AddCommunity(CommunityType::Primary, &city);
                //m_communities[community.getID()] = community
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
                Coordinate cc = city.second.GetCoordinates();
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

void GeoGrid::DefragmentSmallestCities(double X, double Y, const vector<double> &p_vec, stride::util::RNManager &rndm) {
    // Step 1: find all cities that have less then Y% of the population
    // pop_cap: if the population of a city are smaller or equal to this number we defragment them
    auto pop_cap = (unsigned int) round((m_total_pop/100)*Y);
    vector<City*> defrag_cty;
    for (auto& it: m_cities){
        if (it.second.GetPopulation() <= pop_cap) defrag_cty.emplace_back(&it.second);
    }

    // Step2: Decide wich X% of cities to delete
    auto to_defrag = (unsigned int)round((defrag_cty.size()/100)*X);
    while(defrag_cty.size() > to_defrag){
        trng::uniform_int_dist distr(0,(unsigned int)defrag_cty.size() - 1 );
        defrag_cty.erase(defrag_cty.begin() + rndm.GetGenerator(distr)());
    }

    // Step 3: replace X% of these cities
    map<unsigned int, geogen::City> fragments;
    vector<unsigned int> amount_to_frag = generate_random(p_vec, rndm,(unsigned int) defrag_cty.size());
    unsigned int counter = 0;
    for (auto &it: defrag_cty){
        auto city = *it;

        for (unsigned int i =0; i < amount_to_frag[counter]; i++){
            auto new_id = m_cities.end()->second.GetId() + i;
            auto new_coordinates = city.GetCoordinates();
            new_coordinates.latitude += pow(-1, i)*(0.1*i);
            new_coordinates.longitude += pow(-1, i)*(0.1*i);
            new_coordinates.x += pow(-1, i)*(0.1*i);
            new_coordinates.y += pow(-1, i)*(0.1*i);
            auto new_name = city.GetName();
            stringstream ss;
            ss<<i;
            string s;
            ss>>s;
            new_name += s;

            m_cities.insert(pair<unsigned int,City>(new_id,geogen::City(new_id, city.GetProvince(),
                                                                        city.GetPopulation()/amount_to_frag[counter],
                                                                        new_coordinates, new_name)));
            counter++;
        }
        m_cities.erase(it->GetId());

    }

}

} // namespace geogen
