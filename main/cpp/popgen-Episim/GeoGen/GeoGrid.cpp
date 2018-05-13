//
// Created by beau on 3/5/18.
//

#include "GeoGrid.h"


using namespace std;

namespace stride {


void GeoGrid::GetMainFractions()
{
        unsigned int schooled = 0;
        unsigned int workers1 = 0;
        unsigned int workers2 = 0;
        unsigned int toddlers = 0;
        unsigned int oldies   = 0;

        // collapse to apply on the nested loops.
        // reduction, make var copies, + them together at thread merge
#pragma openmp parallel for collapse(3)\
        num_threads(m_config_pt.get<unsigned int>("run/num_threads");)\
        reduction(+:schooled, workers1, workers2, toddlers, oldies)
        for (auto &m_model_household : m_model_households) {
            for (auto house = m_model_household.second.begin(); house < m_model_household.second.end(); house++) {
                for (auto age = house->begin() ; age < house->end(); age++) {
                    // Ordered these if-else if construction to fall as quickly as possible
                    // in the (statistically) most likely age-category...
                    if (*age >= 26 and *age < 65)
                        workers2 += 1;
                    else if (*age >= 3 and *age < 18)
                        schooled += 1;
                    else if (*age >= 18 and *age < 26)
                        workers1 += 1;
                    else if (*age >= 65)
                        oldies += 1;
                    else
                        toddlers += 1;
                }
            }
        }
        double total                        = schooled + workers1 + workers2 + toddlers + oldies;
        m_fract_map[Fractions::SCHOOLED]    = schooled / total;
        m_fract_map[Fractions::YOUNG]       = workers1 / total;
        m_fract_map[Fractions::MIDDLE_AGED] = workers2 / total;
        m_fract_map[Fractions::TODDLERS]    = toddlers / total;
        m_fract_map[Fractions::OLDIES]      = oldies / total;
}

//deprecated?
/*void GeoGrid::GetAgeFractions(vector<double>& popfracs)
{
        for (auto& category : AgeList)
                popfracs.emplace_back(m_fract_map[category]);
}*/

void GeoGrid::ClassifyNeighbours()
{
        // I believe we should be making use of boost's geometry queries here...
#pragma openmp simd for collapse(3)\
        num_threads(m_config_pt.get<unsigned int>("run/num_threads");)
        for (auto& cityA : m_cities) {
                for (auto& cityB : m_cities) {
                        // truncating distance on purpose to avoid using floor-function
                        unsigned int distance = (unsigned int)
                            cityB.second.GetCoordinates().GetDistance(cityA.second.GetCoordinates());
                        // mind that the categories go as follows [0, initial_radius), [initial_radius,
                        // 2*initial_radius), etc.
                        unsigned int category = m_initial_search_radius;
                        // i believe the following code is more efficient than the alternative code below...
                        while ((distance / category) > 0)
                                category <<= 1; // equivalent to multiplying by 2
                        for( auto type : CommunityTypes ) {
                            if( cityB.second.HasCommunityType(type) )
                                m_neighbours_in_radius[cityA.first][category][type].emplace_back(&cityB.second);
                        }
                }
        }
}

GeoGrid::GeoGrid()
        : m_initial_search_radius(0), m_total_pop(0), m_model_pop(0), m_school_count(0),
          m_population(nullptr), m_initialized(false), m_rng(nullptr), m_random_ages(false)
{
        for( auto frac : FractionList )
            m_fract_map[frac] = 0;

        for( auto size : SizeList )
            m_sizes_map[size] = 0;
}

GeoGrid::GeoGrid(const boost::property_tree::ptree& p_tree, util::RNManager* rng)
        : GeoGrid()
{
    Initialize(p_tree, rng);
    GenerateAll();
}

void GeoGrid::Initialize(const boost::filesystem::path& configFile, util::RNManager* rng, const bool override)
{
        boost::property_tree::ptree pTree;
        boost::filesystem::path config;
        config = file_exists(configFile) ? configFile : util::FileSys::GetConfigDir() / configFile;
        config = file_exists(config) ? config : util::FileSys::GetConfigDir() / "run_default.xml";
        boost::property_tree::read_xml(config.string(), pTree);
        if( override )
        {
            pTree.put("run.contact_output_file", false);
            pTree.put("run.rng_type", "lcg64");
        }
        Initialize(pTree, rng);
}

void GeoGrid::InitOutputStuff()
{
    if (m_config_pt.get<string>("run.output_prefix", "").empty()) {
        m_config_pt.put("run.output_prefix", util::TimeStamp().ToTag().append("-PopGen_Standalone/"));
        if( !m_config_pt.get<bool>("run.contact_output_file", false) )
            return;
        if (util::FileSys::IsDirectoryString(m_config_pt.get<string>("run.output_prefix"))) {
            try {
                boost::filesystem::create_directories(m_config_pt.get<string>("run.output_prefix"));
            } catch (exception& e) {
                cerr << "GeoGrid::Initialize> Exception creating directory:  {}"
                     << m_config_pt.get<string>("run.output_prefix") << endl;
                throw;
            }
        }
    }
}

void GeoGrid::AddPopgenPtree()
{
    m_config_pt.put<bool>("run.random_geopop", true); //Set this value to true for Population::Create
    const auto file_name        = m_config_pt.get<string>("run.geopop_file", "geogen_default.xml");
    const auto use_install_dirs = m_config_pt.get<bool>("run.use_install_dirs");
    const auto file_path        = (use_install_dirs) ? util::FileSys::GetConfigDir() /= file_name : file_name;
    boost::property_tree::ptree popgen;
    boost::property_tree::read_xml(file_path.string(), popgen);
    m_config_pt.add_child("run.popgen", popgen);
    m_config_pt.sort();
}

void GeoGrid::ReadFractionsAndSizes()
{
    m_fract_map[Fractions::STUDENTS] = abs(m_config_pt.get<double>("run.popgen.pop_info.fraction_students", 0.5));
    m_fract_map[Fractions::COMMUTING_STUDENTS] =
            abs(m_config_pt.get<double>("run.popgen.pop_info.fraction_commuting_students", 0.5));
    m_fract_map[Fractions::ACTIVE] = abs(m_config_pt.get<double>("run.popgen.pop_info.fraction_active_workers", 0.5));
    m_fract_map[Fractions::COMMUTING_WORKERS] =
            abs(m_config_pt.get<double>("run.popgen.pop_info.fraction_commuting_workers", 0.5));

    m_sizes_map[Sizes::AVERAGE_CP]  = (unsigned int)abs(m_config_pt.get<long>("run.popgen.contactpool_info.average_size", 20));
    m_sizes_map[Sizes::SCHOOLS]     = (unsigned int)abs(m_config_pt.get<long>("run.popgen.contactpool_info.school.size", 500));
    m_sizes_map[Sizes::COLLEGES]    = (unsigned int)abs(m_config_pt.get<long>("run.popgen.contactpool_info.college.size", 3000));
    m_sizes_map[Sizes::MAXLC]       = (unsigned int)abs(m_config_pt.get<long>("run.popgen.contactpool_info.college.cities", 10));
    m_sizes_map[Sizes::COMMUNITIES] = (unsigned int)abs(m_config_pt.get<long>("run.popgen.contactpool_info.community.size", 2000));
    m_sizes_map[Sizes::WORKPLACES]  = (unsigned int)abs(m_config_pt.get<long>("run.popgen.contactpool_info.workplace.size", 20));
}

void GeoGrid::ReadDataFiles()
{
    // reading the cities data file
    const auto base_path  = (m_config_pt.get<bool>("run.use_install_dirs")) ? util::FileSys::GetDataDir() : "";
    string city_file      = m_config_pt.get("run.popgen.data_files.cities", "flanders_cities.csv");
    string commuting_file = m_config_pt.get("run.popgen.data_files.commuting", "flanders_commuting.csv");
    string household_file = m_config_pt.get("run.popgen.data_files.households", "households_flanders.xml");

    m_model_households = parser::ParseHouseholds(base_path / household_file);
    GetMainFractions();

    parser::ParseCities(base_path / city_file, m_cities, m_model_pop);
    parser::ParseCommuting(base_path / commuting_file, m_cities, m_fract_map);

}

void GeoGrid::EnforceEnsures()
{
    // rounding errors cause the first ensure to fail in some conditions...
    // however, is this first ENSURE necessary?
    // it should never fail since we decude the fractions from the households,
    // so removed the correspronding death test until we find a better test...
    double totalfrac = m_fract_map[Fractions::YOUNG] + m_fract_map[Fractions::MIDDLE_AGED] +
                       m_fract_map[Fractions::TODDLERS] + m_fract_map[Fractions::OLDIES] +
                       m_fract_map[Fractions::SCHOOLED];
    ENSURE(fabs(totalfrac - 1) < constants::EPSILON, "Pop frac should equal 1");
    ENSURE(1 >= m_fract_map[Fractions::STUDENTS] and m_fract_map[Fractions::STUDENTS] >= 0,
           "Student fraction must be between 0 and 1");
    ENSURE(1 >= m_fract_map[Fractions::COMMUTING_STUDENTS] and m_fract_map[Fractions::COMMUTING_STUDENTS] >= 0,
           "Student Commuting fraction must be between 0 and 1");
    ENSURE(1 >= m_fract_map[Fractions::ACTIVE] and m_fract_map[Fractions::ACTIVE] >= 0,
           "Active workers fraction must be between 0 and 1");
    ENSURE(1 >= m_fract_map[Fractions::COMMUTING_WORKERS] and m_fract_map[Fractions::COMMUTING_WORKERS] >= 0,
           "Commuting workers fraction must be between 0 and 1");
    // Capping pool size at 1000, gotta ask the professor what the actual cap should be...
    ENSURE(m_sizes_map[Sizes::AVERAGE_CP] > 0 and m_sizes_map[Sizes::AVERAGE_CP] <= 1000,
           "Contactpool's size must be bigger than 0 and smaller than or equal to 1000");

}

void GeoGrid::Initialize(const boost::property_tree::ptree& p_tree, util::RNManager* rng)
{
        //If an incorrect ptree is passed, this function may throw an exception...
        m_config_pt = p_tree;
        InitOutputStuff();
        AddPopgenPtree();
        ReadFractionsAndSizes();
        ReadDataFiles();

        m_total_pop = m_config_pt.get<unsigned int>("run.popgen.pop_info.pop_total", 4341923);
        m_population = Population::Create(m_config_pt);

        m_random_ages = m_config_pt.get<bool>("run.popgen.pop_info.random_ages", false);
        m_initial_search_radius = m_config_pt.get<unsigned int>("run.popgen.neighbour_classification.initial_search_radius", 10U);

        // Setting up RNG
        if( rng == nullptr ) {
            unsigned long seed = (unsigned long)abs(m_config_pt.get("run.rng_seed", 0));
            string        type = m_config_pt.get("run.rng_type", "mrg2");
            default_generator.Initialize(util::RNManager::Info(type, seed));
        }
        //else we're assuming the RNG was initialized already...
        m_rng = (rng) ? rng : &default_generator;
        EnforceEnsures();
        m_initialized = true;
}

void GeoGrid::Reset()
{
        //Reset causes a crash if called multiple times like in the Ctor-test,
        // I have no clue why...
        m_initialized = false;
        m_population = nullptr;
        for( auto frac : FractionList )
            m_fract_map[frac] = 0;
        for( auto size : SizeList )
            m_sizes_map[size] = 0;
        m_model_households.clear();
        m_cities.clear();
        m_neighbours_in_radius.clear();
        m_initial_search_radius = 0;
        m_total_pop = 0;
        m_model_pop = 0;
        m_school_count = 0;
        m_cities_with_college.clear();
        m_random_ages = false;
        m_config_pt.clear();
        m_rng = nullptr;
}

void GeoGrid::GenerateAll()
{
        GenerateColleges();
        GenerateWorkplaces();
        GenerateSchools();
        GenerateCommunities();
        ClassifyNeighbours();
}

void GeoGrid::GenerateSchools()
{
        REQUIRE(m_initialized, "Must call GeoGrid::Initialize() before generation.");

        REQUIRE(m_fract_map[Fractions::SCHOOLED] <= 1, "Schooled fraction can't be bigger then 1.");
        REQUIRE(m_fract_map[Fractions::SCHOOLED] >= 0, "Schooled fraction can't be negative.");
        REQUIRE(m_sizes_map[Sizes::SCHOOLS] > 0, "The school's average size must be strictly positive.");
        // Calculating extra data
        const double amount_schooled = m_total_pop * m_fract_map[Fractions::SCHOOLED];
        // ceil because we want to at least build 1 school
        auto amount_of_schools = (const unsigned int)ceil(amount_schooled / m_sizes_map[Sizes::SCHOOLS]);
        // Determine number of contactpools
        auto cps = ceil((double)m_sizes_map[Sizes::SCHOOLS] / m_sizes_map[Sizes::AVERAGE_CP]);

        // Setting up to divide the schools to cities
        vector<double> p_vec;
        vector<City*>  c_vec;
        for (auto& it : m_cities) {
                c_vec.emplace_back(&it.second);
                p_vec.emplace_back(it.second.GetPopulation());
        }

        auto rndm_vec = generate_random(p_vec, m_rng, amount_of_schools);
        // assign schools to cities according to our normal distribution
        for (unsigned int i = 0; i < amount_of_schools; i++) {
                m_school_count++;
                City&      chosen_city = *c_vec[rndm_vec[i]];
                Community& nw_school   = chosen_city.AddCommunity(CommunityType::School);

                // Add contactpools
                for (auto j = 0; j < cps; j++)
                        nw_school.AddContactPool(m_population->GetContactPoolSys());
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
        if (lc.size() < m_sizes_map[Sizes::MAXLC])
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
        REQUIRE(m_initialized, "Must call GeoGrid::Initialize() before generation.");

        // need m_maxlc largest cities, largest determined by number of people in the city...
        // After deducing fractions from households, these should never fail,
        // they also become difficult to test since we can no longer play with the fractions,
        // gotta come up with new tests for this...
        REQUIRE(m_fract_map[Fractions::STUDENTS] >= 0, "Student fraction can't be negative");
        REQUIRE(m_fract_map[Fractions::STUDENTS] <= 1, "Student fraction can't be more then 100%");
        REQUIRE(m_fract_map[Fractions::YOUNG] >= 0, "Worker fraction can't be negative");
        REQUIRE(m_fract_map[Fractions::YOUNG] <= 1, "Worker fraction can't be more then 100%");
        for (auto& it : m_cities) {
                AdjustLargestCities(m_cities_with_college, it.second);
        }

        // Determine number of contactpools
        auto cps = ceil((double)m_sizes_map[Sizes::COLLEGES] / m_sizes_map[Sizes::AVERAGE_CP]);

        double students = m_total_pop * m_fract_map[Fractions::YOUNG] * m_fract_map[Fractions::STUDENTS];
        auto nrcolleges = (unsigned int)ceil(students / m_sizes_map[Sizes::COLLEGES]);
        vector <double> p_vec;
        for (auto& it : m_cities_with_college) {
            p_vec.emplace_back(it->GetPopulation());
        }

        vector <unsigned int> lottery_vec = generate_random(p_vec, m_rng, nrcolleges);

        for (unsigned int i = 0; i < nrcolleges; i++){
            auto cty = m_cities_with_college[lottery_vec[i]];

            Community& college = cty->AddCommunity(CommunityType::College);
            for (auto j = 0; j < cps; j++){
                college.AddContactPool(m_population->GetContactPoolSys());
            }
        }
}

void GeoGrid::GenerateWorkplaces()
{
        REQUIRE(m_initialized, "Must call GeoGrid::Initialize() before generation.");

        // We create the vec we will choose our city_id's out of.
        // Meaning a citiy has a probality to get assigned a workplace equal to the fraction
        // of people working IN the city (not the active working pop in the city).
        // We have to account for the commuters in the city.

        double possible_workers_frac = (m_fract_map[Fractions::MIDDLE_AGED] +
                                        m_fract_map[Fractions::YOUNG] * (1 - m_fract_map[Fractions::STUDENTS]));
        double active_workers_frac = possible_workers_frac * m_fract_map[Fractions::ACTIVE];

        vector<double> lottery_vec; // vector of relative probabillitys
        vector<City*>  c_vec;       // we will use this to vec to map the city to a set of sequential numbers 0...n
        for (auto& it : m_cities) {

                // Colleges are the only communities that should be present at this time...
                const bool no_colleges = it.second.GetAllCommunities().empty();
                // I believe possible_workers_frac is all we need because in-commuters are definitely active...
                double in_commuters_modifier = (no_colleges) ? 1 : possible_workers_frac;

                auto work_pop = (it.second.GetPopulation() * active_workers_frac +
                                 it.second.GetTotalInCommutersCount() * in_commuters_modifier -
                                 it.second.GetTotalOutCommutersCount() * possible_workers_frac);
                // out-commuters should allways be modified because there can always be students present
                // for in-commuters this is only true if this city contains colleges
                // note that commuters should always be active workers or students

                // Inserting the amount of id's of the city equal to the pop working in the city
                c_vec.emplace_back(&it.second);
                lottery_vec.emplace_back(work_pop);
        }

        // Now we calculate how many workplaces we have to create.
        double allworkers           = active_workers_frac * m_total_pop;
        auto   number_of_workplaces = (unsigned int)ceil(allworkers / m_sizes_map[Sizes::WORKPLACES]);
        auto   rndm_vec             = generate_random(lottery_vec, m_rng, number_of_workplaces);

        auto cps = ceil((double)m_sizes_map[Sizes::WORKPLACES] / m_sizes_map[Sizes::AVERAGE_CP]);

        // Now we will place each workplace randomly in our city, making use of our lottery vec.
        for (unsigned int i = 0; i < number_of_workplaces; i++) {
                City*      chosen_city  = c_vec[rndm_vec[i]];
                Community& nw_workplace = chosen_city->AddCommunity(CommunityType::Work);
                // A workplace has a contactpool.
                for (auto j = 0; j < cps; j++) {
                    nw_workplace.AddContactPool(m_population->GetContactPoolSys());
                }
        }
}

// Communities need to be distributed according to the relative population size.
void GeoGrid::GenerateCommunities()
{
        REQUIRE(m_initialized, "Must call GeoGrid::Initialize() before generation.");

        // Determine number of contactpools
        auto cps = ceil((double)m_sizes_map[Sizes::COMMUNITIES] / m_sizes_map[Sizes::AVERAGE_CP]);

        // First we need to determine the total number of communities to be used.
        auto total_communities = (unsigned int)ceil((double)m_total_pop / m_sizes_map[Sizes::COMMUNITIES]);

        vector<double> p_vec;
        vector<City*>  c_vec;
        for (auto& it : m_cities) {
                c_vec.emplace_back(&it.second);
                p_vec.emplace_back(it.second.GetPopulation());
        }

        auto rndm_vec = generate_random(p_vec, m_rng, 2*total_communities);

        for (unsigned int i = 0; i < 2*total_communities; i++) {
                City&      chosen_city1  = *c_vec[rndm_vec[i++]];
                City&      chosen_city2  = *c_vec[rndm_vec[i]];
                Community& nw_pcommunity = chosen_city1.AddCommunity(CommunityType::Primary);
                Community& nw_scommunity = chosen_city2.AddCommunity(CommunityType::Secondary);
                // Add contactpools for secondary community...
                for (auto j = 0; j < cps; j++) {
                        nw_pcommunity.AddContactPool(m_population->GetContactPoolSys());
                        nw_scommunity.AddContactPool(m_population->GetContactPoolSys());
                }
        }
}

Coordinate GeoGrid::GetCenterOfGrid()
{
        double smallestX    = numeric_limits<double>::max();
        double biggestX     = numeric_limits<double>::lowest();
        double smallestY    = numeric_limits<double>::max();
        double biggestY     = numeric_limits<double>::lowest();
        double smallestLat  = 180.0;  // Highest lat possible (initial)
        double biggestLat   = -180.0; // Lowest lat  possible (initial)
        double smallestLong = 180.0;  // highest long possible (initial)
        double biggestLong  = -180.0; // lowest long possible (initial)

        // Raphael@everyone, excellent example of something we could run in parallel!!!
        // don't understand me wrong, not right now, but later when we're going for beta or in the final phase...
        for (auto& city : m_cities) {
                Coordinate cc = city.second.GetCoordinates();
                if (cc.GetX() < smallestX)
                        smallestX = cc.GetX();
                if (cc.GetX() > biggestX)
                        biggestX = cc.GetX();
                if (cc.GetY() < smallestY)
                        smallestY = cc.GetY();
                if (cc.GetY() > biggestY)
                        biggestY = cc.GetY();
                if (cc.GetLatitude() < smallestLat)
                        smallestLat = cc.GetLatitude();
                if (cc.GetLatitude() > biggestLat)
                        biggestLat = cc.GetLatitude();
                if (cc.GetLongitude()< smallestLong)
                        smallestLong = cc.GetLongitude();
                if (cc.GetLongitude()> biggestLong)
                        biggestLong = cc.GetLongitude();
        }

        double halfX    = (biggestX - smallestX) / 2;
        double halfY    = (biggestY - smallestY) / 2;
        double halfLat  = (biggestLat - smallestLat) / 2;
        double halfLong = (biggestLong - smallestLong) / 2;

        return {(smallestX + halfX), (smallestY + halfY), (smallestLong + halfLong), (smallestLat + halfLat)};
}

void GeoGrid::DefragmentSmallestCities(double X, double Y, const vector<double>& p_vec)
{
        // Step 1: find all cities that have less then Y% of the population
        // pop_cap: if the population of a city are smaller or equal to this number we defragment them
        auto          pop_cap = (unsigned int)round(m_total_pop * Y);
        vector<City*> defrag_cty;
        for (auto& it : m_cities) {
                if (it.second.GetPopulation() <= pop_cap)
                        defrag_cty.emplace_back(&it.second);
        }

        // Step2: Decide wich X% of cities to delete
        auto to_defrag = (unsigned int)round(defrag_cty.size() * X);
        while (defrag_cty.size() > to_defrag) {
                trng::uniform_int_dist distr(0, (unsigned int)defrag_cty.size() - 1);
                defrag_cty.erase(defrag_cty.begin() + m_rng->GetGenerator(distr)());
        }

        // Step 3: replace X% of these cities
        vector<unsigned int> amount_to_frag = generate_random(p_vec, m_rng, (unsigned int)defrag_cty.size());
        defrag_cty.shrink_to_fit();
        unsigned int counter = 0;
        for (auto& it : defrag_cty) {
                // We add 2 to the amount to defrag, bcs we want to defrag in atleast 2 parts
                for (unsigned int i = 0; i < amount_to_frag[counter] + 2; i++) {

                        auto new_id    = m_cities.rbegin()->first + 1;
                        auto coords    = it->GetCoordinates();
                        double newLat  = coords.GetLatitude() + pow(-1, i) * (0.1 * i);
                        double newLong = coords.GetLongitude() + pow(-1, i) * (0.1 * i);
                        double newX    = coords.GetX() + pow(-1, i) * (0.1 * i);
                        double newY    = coords.GetY() + pow(-1, i) * (0.1 * i);
                        auto new_name = it->GetName();
                        new_name += to_string(i);
                        m_cities.insert(pair<unsigned int, City>(
                            new_id, City(new_id, it->GetProvince(), it->GetPopulation() / (amount_to_frag[counter] + 2),
                                         Coordinate(newX, newY, newLong, newLat), new_name)));
                }
                counter++;
                m_cities.erase(it->GetId());
        }
        // cout << m_cities.size() << endl;
}

const vector<City*>& GeoGrid::GetCitiesWithinRadiusWithCommunityType(const City& origin, unsigned int radius, CommunityType type)
{
        if (!m_neighbours_in_radius[origin.GetId()].count(radius)) {
                unsigned int next_smaller = m_initial_search_radius;
                while ((radius / next_smaller) > 1)
                        next_smaller <<= 1; // equivalent to multiplying by 2.
                unsigned int next_bigger = next_smaller << 1;
                if ((next_bigger - radius) >= (radius - next_smaller))
                        radius = next_smaller;
                else
                        radius = next_bigger;
                //make sure radius does not exceed the limit
                radius = min(radius, m_neighbours_in_radius[origin.GetId()].rbegin()->first);
        }
        return m_neighbours_in_radius[origin.GetId()][radius][type];
}

void GeoGrid::WritePopToFile(const string &fname) const
{
    std::ofstream file;
    file.open ((m_config_pt.get<string>("run.output_prefix") + fname).c_str(), std::ofstream::out);
    file << "\"age\",\"household_id\",\"school_id\",\"work_id\",\"primary_community\",\"secundary_community\"" << endl;
    file << *m_population << endl;
    file.close();
}

} // namespace stride
