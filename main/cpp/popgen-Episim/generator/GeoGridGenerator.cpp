//
// Created by Raphael A. on 18/05/2018.
//

#include "GeoGridGenerator.h"
#include "pop/Population.h"
#include "pop/SurveySeeder.h"
#include "popgen-Episim/model/GeoGrid.h"
#include "popgen-Episim/util/DesignByContract.h"
#include "util/CSV.h"
#include "util/FileSys.h"
#include "util/TimeStamp.h"

#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry/strategies/geographic/distance_andoyer.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

using namespace std;

namespace stride {

shared_ptr<GeoGrid> GeoGridGenerator::Generate(const boost::filesystem::path &configFile, const bool contactFile)
{
    boost::property_tree::ptree pTree;
    boost::filesystem::path config;
    config = file_exists(configFile) ? configFile : util::FileSys::GetConfigDir() / configFile;
    config = file_exists(config) ? config : util::FileSys::GetConfigDir() / "run_default.xml";
    boost::property_tree::read_xml(config.string(), pTree, boost::property_tree::xml_parser::trim_whitespace);
    pTree.put("run.contact_output_file", contactFile);
    return Generate(pTree);
}

shared_ptr<GeoGrid> GeoGridGenerator::Generate(const boost::property_tree::ptree &pTree)
{

    m_cid_generator = 1;
    struct make_shared_enabler : public GeoGrid
    {
    };
    m_grid = make_shared<make_shared_enabler>();
    //If an incorrect ptree is passed, this function may throw an exception...
    m_grid->m_config_pt = pTree;
    InitOutputStuff();
    AddPopgenPtree();
    ReadFractionsAndSizes();
    ReadDataFiles();

    const auto& pt = m_grid->m_config_pt;
    omp_set_num_threads(pt.get<int>("run.num_threads"));
    m_grid->m_total_pop = pt.get<unsigned int>("run.popgen.pop_info.pop_total", 4341923);
    m_grid->m_population = Population::Create(pt);

    m_grid->m_random_ages = pt.get<bool>("run.popgen.pop_info.random_ages", false);
    m_grid->m_initial_search_radius =
            pt.get<unsigned int>("run.popgen.neighbour_classification.initial_search_radius", 10U);

    unsigned long seed = pt.get<unsigned long>("run.rng_seed", 1UL);
    string type = pt.get("run.rng_type", "mrg2");
    unsigned int numThreads = pt.get<unsigned int>("run.num_threads", 1U);
    m_grid->m_rng.Initialize(util::RNManager::Info{type, seed, "", numThreads});
    EnsureConsistency();

    if( pt.get<bool>("run.prebuilt_geopop", false) )
    {
        CommunitiesFromFile(pt.get<string>("run.communities_file", "communities.csv"));
        PopulationFromFile(pt.get<string>("run.population_file", "population.csv"));
        SurveySeeder(pt, m_grid->m_rng).Seed(m_grid->m_population);
        //surveyseeding should be done at a more appropriate location, right now it's all over the place...
        ClassifyNeighbours();
        m_grid->m_rng.StateFromFile(pt.get<string>("run.rng_state_file", "RNG-state.xml"));

    }
    else
        GenerateAll();

    return m_grid;
}

void GeoGridGenerator::GetMainFractions()
{
    auto& fmap = m_grid->m_fract_map;
    for (const auto& houses : m_grid->m_model_households)
        for (const auto &house : houses.second)
            for (const auto &age : house)
                fmap[get_category(age)] += 1;
    double total = 0;
    for( auto category : AgeList )
        total += fmap[category];
    for( auto category : AgeList )
        fmap[category] /= total;
}

void GeoGridGenerator::InitOutputStuff()
{
    auto& pt = m_grid->m_config_pt;
    if (pt.get<string>("run.output_prefix", "").empty()) {
        pt.put("run.output_prefix", util::TimeStamp().ToTag().append("-PopGen_Standalone/"));
        if( !pt.get<bool>("run.contact_output_file", false) )
            return;
        if (util::FileSys::IsDirectoryString(pt.get<string>("run.output_prefix"))) {
            try {
                boost::filesystem::create_directories(pt.get<string>("run.output_prefix"));
            } catch (exception& e) {
                cerr << "GeoGrid::Initialize> Exception creating directory:  {}"
                     << pt.get<string>("run.output_prefix") << endl;
                throw;
            }
        }
    }
}

void GeoGridGenerator::AddPopgenPtree()
{
    auto& pt = m_grid->m_config_pt;
    pt.put<bool>("run.random_geopop", true); //Set this value to true for Population::Create
    const auto file_name        = pt.get<string>("run.geopop_file", "geogen_default.xml");
    const auto use_install_dirs = pt.get<bool>("run.use_install_dirs");
    const auto file_path        = (use_install_dirs) ? util::FileSys::GetConfigDir() /= file_name : file_name;
    boost::property_tree::ptree popgen;
    boost::property_tree::read_xml(file_path.string(), popgen, boost::property_tree::xml_parser::trim_whitespace);
    pt.add_child("run.popgen", popgen);
}

void GeoGridGenerator::ReadFractionsAndSizes()
{
    const auto& pt = m_grid->m_config_pt;
    auto& fmap = m_grid->m_fract_map;
    auto& smap = m_grid->m_sizes_map;
    fmap[Fractions::STUDENTS] = abs(pt.get<double>("run.popgen.pop_info.fraction_students", 0.5));
    fmap[Fractions::COMMUTING_STUDENTS] = abs(pt.get<double>("run.popgen.pop_info.fraction_commuting_students", 0.5));
    fmap[Fractions::ACTIVE] = abs(pt.get<double>("run.popgen.pop_info.fraction_active_workers", 0.5));
    fmap[Fractions::COMMUTING_WORKERS] = abs(pt.get<double>("run.popgen.pop_info.fraction_commuting_workers", 0.5));

    smap[Sizes::AVERAGE_CP]  = (unsigned int)abs(pt.get<int>("run.popgen.contactpool_info.average_size", 20));
    smap[Sizes::SCHOOLS]     = (unsigned int)abs(pt.get<int>("run.popgen.contactpool_info.school.size", 500));
    smap[Sizes::COLLEGES]    = (unsigned int)abs(pt.get<int>("run.popgen.contactpool_info.college.size", 3000));
    smap[Sizes::MAXLC]       = (unsigned int)abs(pt.get<int>("run.popgen.contactpool_info.college.cities", 10));
    smap[Sizes::COMMUNITIES] = (unsigned int)abs(pt.get<int>("run.popgen.contactpool_info.community.size", 2000));
    smap[Sizes::WORKPLACES]  = (unsigned int)abs(pt.get<int>("run.popgen.contactpool_info.workplace.size", 20));
}

void GeoGridGenerator::ReadDataFiles()
{
    // reading the cities data file
    const auto& pt        = m_grid->m_config_pt;
    const auto basePath  = pt.get<bool>("run.use_install_dirs", true) ? util::FileSys::GetDataDir() : "";
    string cityFile      = pt.get("run.popgen.data_files.cities", "flanders_cities.csv");
    string commutingFile = pt.get("run.popgen.data_files.commuting", "flanders_commuting.csv");
    string householdFile = pt.get("run.popgen.data_files.households", "households_flanders.xml");

    m_grid->m_model_households = parser::ParseHouseholds(basePath / householdFile);
    GetMainFractions();

    parser::ParseCities(basePath / cityFile, m_grid->m_cities, m_grid->m_model_pop, m_grid->m_rtree);
    parser::ParseCommuting(basePath / commutingFile, m_grid->m_cities, m_grid->m_fract_map);

}

void GeoGridGenerator::EnsureConsistency()
{
    const auto& fmap = m_grid->m_fract_map;
    const auto& smap = m_grid->m_sizes_map;
    double totalfrac = fmap.at(Fractions::YOUNG) + fmap.at(Fractions::MIDDLE_AGED) +
                       fmap.at(Fractions::TODDLERS) + fmap.at(Fractions::OLDIES) +
                       fmap.at(Fractions::SCHOOLED);
    ENSURE(fabs(totalfrac - 1) < constants::EPSILON, "Pop frac should equal 1");
    ENSURE(1 >= fmap.at(Fractions::STUDENTS) and fmap.at(Fractions::STUDENTS) >= 0,
           "Student fraction must be between 0 and 1");
    ENSURE(1 >= fmap.at(Fractions::COMMUTING_STUDENTS) and fmap.at(Fractions::COMMUTING_STUDENTS) >= 0,
           "Student Commuting fraction must be between 0 and 1");
    ENSURE(1 >= fmap.at(Fractions::ACTIVE) and fmap.at(Fractions::ACTIVE) >= 0,
           "Active workers fraction must be between 0 and 1");
    ENSURE(1 >= fmap.at(Fractions::COMMUTING_WORKERS) and fmap.at(Fractions::COMMUTING_WORKERS) >= 0,
           "Commuting workers fraction must be between 0 and 1");
    // Capping pool size at 1000, gotta ask the professor what the actual cap should be...
    ENSURE(smap.at(Sizes::AVERAGE_CP) > 0 and smap.at(Sizes::AVERAGE_CP) <= 1000,
           "A contactpool's size must be bigger than 0 and smaller than or equal to 1000");
    ENSURE(smap.at(Sizes::SCHOOLS) > 0, "A school's average size must be strictly positive.");
    ENSURE(smap.at(Sizes::COLLEGES) > 0, "A college's average size must be strictly positive.");
    ENSURE(smap.at(Sizes::MAXLC) > 0, "The number of cities with a college must be strictly positive.");
    ENSURE(smap.at(Sizes::WORKPLACES) > 0, "A workplace's average size must be strictly positive.");
    ENSURE(smap.at(Sizes::COMMUNITIES) > 0, "A community's average size must be strictly positive.");
    ENSURE(m_grid->m_initial_search_radius > 0, "Initial search radius must be strictly positive.");
}

void GeoGridGenerator::CommunitiesFromFile(const string &fname)
{
    const auto& smap = m_grid->m_sizes_map;
    util::CSV csv(fname);
#pragma omp parallel for
    for (auto row = csv.begin(); row < csv.end(); row ++)
    {
        auto  id   = row->GetValue<size_t>("community_id");
        auto  type = row->GetValue<string>("community_type");
        auto  cid  = row->GetValue<unsigned int>("city_id");
        auto& c    = m_grid->m_cities.at(cid).AddCommunity(id, CommunityType::ToType(type));
        Sizes size = CommunityType::ToSizes(CommunityType::ToType(type));
        for( unsigned int i=0; i < ceil((double)smap.at(size) / smap.at(Sizes::AVERAGE_CP)); i++ ) {
            c.AddContactPool(m_grid->m_population->GetContactPoolSys());
        }

        if( type == "school" ) {
#pragma atomic
            m_grid->m_school_count++;
        }
        if( type == "college" ) {
#pragma critical(c_college_emplace)
            {
                m_grid->m_cities_with_college.emplace_back(&m_grid->m_cities.at(cid));
            }
        }
    }
}
void GeoGridGenerator::PopulationFromFile(const string &fname)
{
    ContactPoolSys& poolSys = m_grid->m_population->GetContactPoolSys();
    unsigned int pid = 0U;
    util::CSV csv(fname);
#pragma omp parallel for
    for (auto row = csv.begin(); row < csv.end(); row ++)
    {
        const auto age  = row->GetValue<double>("age");
        const auto hid  = row->GetValue<size_t>("household_id");
        const auto sid  = row->GetValue<size_t>("school_id");
        const auto wid  = row->GetValue<size_t>("work_id");
        const auto pcid = row->GetValue<size_t>("primary_community");
        const auto scid = row->GetValue<size_t>("secundary_community");
#pragma omp critical(emplace_bck)
        {
            m_grid->m_population->emplace_back(pid++, age, hid, sid, wid, pcid, scid);
        }
        if( hid > poolSys[ContactPoolType::Id::Household].size() )
        {
            const auto cid  = row->GetValue<unsigned int>("city_id");
#pragma omp critical(add_house_hold)
            {
                m_grid->m_cities.at(cid).AddHousehold(poolSys);
            }
        }
        const Person* p = &m_grid->m_population->back();
        for( auto type : ContactPoolType::IdList )
            if( p->GetPoolId(type) != 0 )
                poolSys[type][p->GetPoolId(type)-1].AddMember(p);
    }
}

void GeoGridGenerator::GenerateAll()
{
    DefragCities();
    GenerateColleges();
    GenerateWorkplaces();
    GenerateSchools();
    GenerateCommunities();
    ClassifyNeighbours();
}

void GeoGridGenerator::GenerateSchools()
{
    const auto& fmap = m_grid->m_fract_map;
    const auto& smap = m_grid->m_sizes_map;

    // Setting up to divide the schools to cities
    vector<double> pVec;
    vector<City*>  cVec;
    for (auto& it : m_grid->m_cities) {
        cVec.emplace_back(&it.second);
        pVec.emplace_back(it.second.GetPopulation());
    }

    auto amountSchooled  = m_grid->m_total_pop * fmap.at(Fractions::SCHOOLED);
    auto amountOfSchools = (const unsigned int)ceil(amountSchooled / smap.at(Sizes::SCHOOLS));
    m_grid->m_school_count += amountOfSchools;
    auto rndmVec = generate_random(pVec, m_grid->m_rng, amountOfSchools);
    AddCommunities(cVec, rndmVec, CommunityType::Id::School);
}

unsigned int GeoGridGenerator::FindSmallest(const vector<City*>& lc)
{
    unsigned int smallest = 0;
    for (unsigned int i = 1; i < lc.size(); i++) {
        if (lc[smallest]->GetPopulation() > lc[i]->GetPopulation())
            smallest = i;
    }
    return smallest;
}

void GeoGridGenerator::AdjustLargestCities(vector<City*>& lc, City& city)
{
    if (lc.size() < m_grid->m_sizes_map[Sizes::MAXLC])
        lc.emplace_back(&city);
    else {
        unsigned int citpop   = city.GetPopulation();
        unsigned int smallest = FindSmallest(lc);
        if (citpop > lc[smallest]->GetPopulation())
            lc[smallest] = &city;
    }
}

void GeoGridGenerator::GenerateColleges()
{
    const auto&    fmap   = m_grid->m_fract_map;
    const auto&    smap   = m_grid->m_sizes_map;
    vector<City*>& cities = m_grid->m_cities_with_college;
    for (auto& it : m_grid->m_cities)
        AdjustLargestCities(cities, it.second);

    vector <double> pVec;
    for (auto it : cities)
        pVec.emplace_back(it->GetPopulation());

    double students = m_grid->m_total_pop * fmap.at(Fractions::YOUNG) * fmap.at(Fractions::STUDENTS);
    auto nrcolleges = (unsigned int)ceil(students / smap.at(Sizes::COLLEGES));
    auto lotteryVec = generate_random(pVec, m_grid->m_rng, nrcolleges);
    AddCommunities(cities, lotteryVec, CommunityType::Id::College);
    // filter out the cities that eventually haven't received colleges
    for( unsigned int i=0; i < cities.size(); i++ )
        if( !cities[i]->HasCommunityType(CommunityType::Id::College) )
            cities.erase(cities.begin()+i--);
}

void GeoGridGenerator::GenerateWorkplaces()
{
    const auto& fmap = m_grid->m_fract_map;
    const auto& smap = m_grid->m_sizes_map;
    double possibleWorkersFrac = (fmap.at(Fractions::MIDDLE_AGED) +
                                    fmap.at(Fractions::YOUNG) * (1 - fmap.at(Fractions::STUDENTS)));
    double activeWorkersFrac = possibleWorkersFrac * fmap.at(Fractions::ACTIVE);

    vector<double> lotteryVec; // vector of relative probabillitys
    vector<City*>  cVec;       // we will use this to vec to map the city to a set of sequential numbers 0...n
    for (auto& it : m_grid->m_cities) {
        const bool hasCollege = it.second.HasCommunityType(CommunityType::Id::College);
        double inCommutersModifier = (hasCollege) ? possibleWorkersFrac : 1;
        // if there's no college in this city, all incoming commuters are workers

        auto workPop = (it.second.GetPopulation() * activeWorkersFrac +
                         it.second.GetTotalInCommutersCount() * inCommutersModifier -
                         it.second.GetTotalOutCommutersCount() * possibleWorkersFrac);
        // out-commuters should allways be modified because there can always be students present
        // for in-commuters this is only true if this city contains colleges
        // note that commuters should always be active workers or students

        // Inserting the amount of id's of the city equal to the pop working in the city
        cVec.emplace_back(&it.second);
        lotteryVec.emplace_back(workPop);
    }

    // Now we calculate how many workplaces we have to create.
    double allworkers           = activeWorkersFrac * m_grid->m_total_pop;
    auto   numberOfWorkplaces = (unsigned int)ceil(allworkers / smap.at(Sizes::WORKPLACES));
    auto   rndmVec             = generate_random(lotteryVec, m_grid->m_rng, numberOfWorkplaces);
    AddCommunities(cVec, rndmVec, CommunityType::Id::Work);
}

// Communities need to be distributed according to the relative population size.
void GeoGridGenerator::GenerateCommunities()
{
    const auto& smap = m_grid->m_sizes_map;
    // First we need to determine the total number of communities to be used.
    auto totalCommunities = (unsigned int)ceil((double)m_grid->m_total_pop / smap.at(Sizes::COMMUNITIES));

    vector<double> pVec;
    vector<City*>  cVec;
    for (auto& it : m_grid->m_cities) {
        cVec.emplace_back(&it.second);
        pVec.emplace_back(it.second.GetPopulation());
    }

    auto rndmVec = generate_random(pVec, m_grid->m_rng, totalCommunities);
    AddCommunities(cVec, rndmVec, CommunityType::Id::Primary);
    rndmVec = generate_random(pVec, m_grid->m_rng, totalCommunities);
    AddCommunities(cVec, rndmVec, CommunityType::Id::Secondary);
}

void GeoGridGenerator::ClassifyNeighbours()
{
    //cout << "Starting classification..." << endl;
    //const clock_t begin_time = clock();
    //this approach without boost's rtree queries is much faster for flanders_cities (327 cities)
    // could be different if we had more cities though, but can we test that?
    auto& nirmap = m_grid->m_neighbours_in_radius;
    auto& cities = m_grid->m_cities;

    vector<unsigned int> keys;

    unsigned int counter = 0;
    for(auto& it: cities){
        keys.emplace_back(it.first);
        counter++;
    }

//#pragma omp parallel for
    for (auto ka = keys.begin(); ka < keys.end(); ka++) {
        auto cityA = cities.at(*ka);
        for (auto& cityB : cities) {
            // truncating distance on purpose to avoid using floor-function
            unsigned int distance =
                    cityB.second.GetCoordinates().GetDistance(cityA.GetCoordinates());
            // mind that the categories go as follows [0, initial_radius), [initial_radius,
            // 2*initial_radius), etc.
            unsigned int category = m_grid->m_initial_search_radius;
            while ((distance / category) > 0)
                category <<= 1; // equivalent to multiplying by 2, just more efficient...
            for( auto type : CommunityType::IdList) {
                if( cityB.second.HasCommunityType(type) )
#pragma critical(nirmap_emplace)
                {
                    nirmap[*ka][category][type].emplace_back(&cityB.second);
                }
            }
        }
    }
    //cout << "Done classifying, time needed = " << double(clock() - begin_time) / CLOCKS_PER_SEC << endl;
}

void GeoGridGenerator::ClassifyNeighbours2()
{
    //cout << "Starting classification..." << endl;
    //const clock_t begin_time = clock();
    auto& nirmap = m_grid->m_neighbours_in_radius;
    auto& cities = m_grid->m_cities;
    for (auto& cityA : cities) {
        unsigned int last = 0;
        unsigned int radius = m_grid->m_initial_search_radius;
        while( radius > last ) { //this while-loop should be optimized...
            std::vector<rtElem> result;
            m_grid->m_rtree.query(
                    bgi::satisfies([&](rtElem const& e)
                                   {double dist = bg::distance(e.first, cityA.second.GetCoordinates().GetLongLat());
                                       return (dist < radius and dist >= last);}),
                    std::back_inserter(result));
            for( const auto& city : result ) {
                for (auto type : CommunityType::IdList) {
                    if( cities.at(city.second).HasCommunityType(type) )
                        nirmap[cityA.first][radius][type].emplace_back(&cities.at(city.second));

                }
            }
            last = radius;
            radius <<= 1; // multiply by 2
        }
    }
    //cout << "Done classifying, time needed = " << double(clock() - begin_time) / CLOCKS_PER_SEC << endl;
}

void GeoGridGenerator::AddCommunities(const vector<City *>& cities, const vector<unsigned int>& indices,
                                      CommunityType::Id type)
{
    const auto& smap = m_grid->m_sizes_map;
    auto cps = ceil((double)smap.at(CommunityType::ToSizes(type)) / smap.at(Sizes::AVERAGE_CP));

//#pragma omp parallel for
    for (unsigned int i = 0; i < indices.size(); i++) {

            Community* nwSchool;

            City &chosen_city = *cities[indices[i]];
#pragma  omp critical(f)
        {
            nwSchool = &chosen_city.AddCommunity(m_cid_generator++, type);
        }

            // Add contactpools
            for (auto j = 0; j < cps; j++)
#pragma omp critical(a)
        {
            nwSchool->AddContactPool(m_grid->m_population->GetContactPoolSys());
        }
    }
}

    void GeoGridGenerator::DefragCities() {

        boost::property_tree::ptree ptree = m_grid->GetConfigPtree();
        if(ptree.get("run.popgen.defrag_cities.is_defrag", false)){

            float X = ptree.get<float>("run.popgen.defrag_cities.X");
            float Y = ptree.get<float>("run.popgen.defrag_cities.Y");

            vector<double> p_vec;
            BOOST_FOREACH(boost::property_tree::ptree::value_type &it, ptree.get_child("run.popgen.defrag_cities.p_vec")) {
                            // The data function is used to access the data stored in a node.
                            try{
                                p_vec.emplace_back(stod(it.second.data()));
                            }
                           catch (exception& e){
                                cout << "invalid in p_vec config" << endl;
                            }
                        }

            m_grid->DefragmentSmallestCities(X, Y, p_vec);
        }

    }

} //namespace stride