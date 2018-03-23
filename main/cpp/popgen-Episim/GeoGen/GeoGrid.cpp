//
// Created by beau on 3/5/18.
//

#include "GeoGrid.h"
#include "trng/uniform_int_dist.hpp"
using namespace std;

namespace geogen {

    GeoGrid::GeoGrid(const boost::filesystem::path & config_file):
            m_generator(0){

        this->m_school_count = 0;
        //Setting up property tree to parse xml config file
        boost::property_tree::ptree p_tree;
        boost::property_tree::read_xml(config_file.string(), p_tree);

        //reading the cities data file
        string base_path = "data/";
        string city_file = p_tree.get("popgen.data_files.cities","flanders_cities.csv");

        string commuting_file =  p_tree.get("popgen.data_files.commuting","flanders_commuting.csv");

        m_cities = parser::ParseCities(base_path + city_file, base_path + commuting_file, true);

        //m_commuting = parser::ParseCommuting(base_path + commuting_file);

        //Generating schools
        //auto total_pop = p_tree.get<unsigned int>("popgen.pop_info.pop_total");
        //specs ask this to be read out of config, but could be calculated directly
        //out of the city file?
        // -> you're right... so let's do it like this:
        //m_total_pop = CountTotalPop();
        //After specifically asking about this, turns out we still need to read it from file...
        //perhaps find a way to verify this number somehow, if the possibility exists of course...
        m_total_pop = this->CountTotalPop();

        m_schooled_frac = p_tree.get<float>("popgen.pop_info.fraction_schooled");
        m_workers1_frac = p_tree.get<float>("popgen.pop_info.fraction_workers1");
        m_workers1_frac = p_tree.get<float>("popgen.pop_info.fraction_workers2");
        m_rest_frac = p_tree.get<float>("popgen.pop_info.fraction_rest");

        m_student_frac = p_tree.get<float>("popgen.pop_info.fraction_students");
        m_commuting_students_frac = p_tree.get<float>("popgen.pop_info.fraction_commuting_students");
        m_active_frac = p_tree.get<float>("popgen.pop_info.fraction_active_workers");
        m_commuting_workers_frac = p_tree.get<float>("popgen.pop_info.fraction_commuting_workers");

        m_school_size = p_tree.get<unsigned int>("popgen.contactpool_info.school.size");
        m_college_size =  p_tree.get<unsigned int>("popgen.contactpool_info.college.size");
        m_maxlc = p_tree.get<unsigned int>("popgen.contactpool_info.college.cities");
        m_community_size = p_tree.get<unsigned int>("popgen.contactpool_info.community.size");
        m_worksplace_size = p_tree.get<unsigned int>("popgen.contactpool_info.workplace.size");


    }

    void GeoGrid::GenerateAll() {
        GenerateSchools();
        GenerateColleges();
        GenerateWorkplaces();
        GenerateCommunities();

    }

    void GeoGrid::GenerateSchools() {

        assert(this->m_schooled_frac<=1);
        assert(this->m_school_size > 0);
        // Calculating extra data
        // rounded because we don't have a fraction of a person
        auto amount_schooled = (const unsigned int) round(m_total_pop * m_schooled_frac);
        // round because we do not build half a school
        auto amount_of_schools = (const unsigned int) round(amount_schooled / m_school_size);


        // Setting up to divide the schools to cities
        vector<unsigned int> pop_id; //We will push the id's of the cities for each pop member.
        for(auto &it: m_cities){
            auto c_schooled_pop = (unsigned int) round(it.second->GetPopulation() * m_schooled_frac);
            pop_id.insert(pop_id.end(), c_schooled_pop, (const unsigned int &) it.first);
        }
        //Note that this way cuz of rounding we lose a couple of schooled ppl.
        //But this shouldn't affect our city divison.

        trng::uniform_int_dist distr(0, (unsigned int) pop_id.size() - 1);

        // assign schools to cities according to our normal distribution
        for(unsigned int i = 0; i < amount_of_schools; i++){
            m_school_count++;
            int index = pop_id[distr(m_generator)];
            shared_ptr<City> chosen_city = m_cities[index];
            shared_ptr<Community> nw_school(new Community(CommunityType::School, chosen_city));
            chosen_city->AddCommunity(nw_school);
            //m_communities[nw_school->getID()] = nw_school
        }


    }

    unsigned int GeoGrid::FindSmallest(const vector<shared_ptr<City>> &lc) {
        unsigned int smallest = 0;
        for (unsigned int i = 1; i < lc.size(); i++) {
            if (lc[smallest]->GetPopulation() > lc[i]->GetPopulation()) smallest = i;
        }
        return smallest;
    }

    void GeoGrid::AdjustLargestCities(vector<shared_ptr<City>> &lc, const shared_ptr<City> &city) {
        if (lc.size() < m_maxlc) lc.push_back(city);
        else {
            unsigned int citpop = city->GetPopulation();
            unsigned int smallest = FindSmallest(lc);
            if (citpop > lc[smallest]->GetPopulation()) lc[smallest] = city;
        }
    }

    void GeoGrid::GenerateColleges() {
        //need 10 largest cities, largest determined by number of people in the city...
        vector <shared_ptr<City>> lc;
        for (auto &it : m_cities) {
            AdjustLargestCities(lc, it.second);
        }

        //generate colleges to the respective cities...
        for (auto &it : lc) {
            //just checking which cities we found...
            //cout << it->GetId() << "   " << it->GetPopulation() << "   " << it->GetName() << endl;

            //so let's go...
            double students = it->GetPopulation()*m_workers1_frac*m_student_frac;
            //doesn't matter if students is a double at this time
            // since this is only an estimate for the number of colleges
            auto nrcolleges = (unsigned int) round(students / m_college_size);
            //is this how we need to calculate the nr of colleges?
            // or did i not understand it properly?
            //cout << students << " students,   " << nrcolleges << " colleges for " << it->GetName() << endl;

            for(unsigned int i = 0; i < nrcolleges; i++) {
                shared_ptr<Community> college = make_shared<Community>(CommunityType::College, it);
                it->AddCommunity(college);
                //m_communities[college->getID()] = college
            }
        }
    }

    void GeoGrid::GenerateWorkplaces() {
        //calculating the required informations

        //double working_population = m_active_frac * m_total_pop;
        //double number_of_workplaces = working_population/m_worksplace_size;

        //dividing workplaces to cities
        for (auto it:m_cities){
            shared_ptr<City> city = it.second;
            unsigned int in_commuters = city->GetNumberOfInCommuters();

            //To be confirmed: everybody commutes, the in-commuters have all the people working in that region,
            //including locals who work in their own region
            //some percentages of the commuters are students
            double working_commuters = m_commuting_workers_frac * in_commuters;
            auto number_of_workplaces =(unsigned int) round(working_commuters / m_worksplace_size);

            for(unsigned int i=0; i<number_of_workplaces; i++){
                shared_ptr<Community> community = make_shared<Community>(CommunityType::Work, city);
                city->AddCommunity(community);


            }

        }

    }

    /// Communities need to be distributed according to the relative population size.
    /// On average a community has 2000 members.
    void GeoGrid::GenerateCommunities() {
        cout<<"start generating communities"<<endl;
        vector<shared_ptr<Community>> primsec_communities;
        /// First we need to determine the total number of communities to be used.
        auto total_communities = ceil(m_total_pop/m_community_size);
        /// Determine how many communities a city should get.
        for (auto it : m_cities){
            /// it: pair<int, City>
            shared_ptr<City> city = it.second;
            /// ratio: the city contains ratio % of the total population.
            double ratio = (city->GetPopulation()/(double)m_total_pop);
            assert(0<ratio<1);
            /// Now we have the ratio, we know that the city has ratio % of all communities.
            auto city_communities = (total_communities*ratio);
            assert(city_communities<total_communities);

            for (int i = 0; i < city_communities; i++){
                /// Since there currently is no real difference between primary and secundary communities we make them all primary.
                shared_ptr<Community> community = make_shared<Community>(CommunityType::Primary, city);
                city->AddCommunity(community);
                //m_communities[community->getID()] = community
            }
        }
    }


    unsigned int GeoGrid::CountTotalPop() const {

        unsigned int counter = 0;
        for (auto &it : m_cities) {
            counter += it.second->GetPopulation();
        }
        return counter;
    }

    const map<int, shared_ptr<City>>& GeoGrid::get_cities(){
        return m_cities;
    }

    unsigned int GeoGrid::GetSchoolCount() const {
        return m_school_count;
    }

    shared_ptr<City> &GeoGrid::operator[](int i) {
        return m_cities[i];
    };

}//namespace geogen