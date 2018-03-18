//
// Created by beau on 3/5/18.
//

#include "GeoGrid.h"

using namespace std;

namespace geogen {

    GeoGrid::GeoGrid(const boost::filesystem::path & config_file) {

        this->school_count = 0;
        //Setting up property tree to parse xml config file
        boost::property_tree::ptree p_tree;
        boost::property_tree::read_xml(config_file.string(), p_tree);

        //reading the cities data file
        string base_path = "data/";
        string city_file = p_tree.get("popgen.data_files.cities","flanders_cities.csv");
        m_cities = parser::parse_cities(base_path + city_file);

        string commuting_file =  p_tree.get("popgen.data_files.commuting","flanders_commuting.csv");
        m_commuting = parser::parse_commuting(base_path + commuting_file);

        //Generating schools
        //auto total_pop = p_tree.get<unsigned int>("popgen.pop_info.pop_total");
        //specs ask this to be read out of config, but could be calculated directly
        //out of the city file?
        // -> you're right... so let's do it like this:
        //m_total_pop = count_total_pop();
        //After specifically asking about this, turns out we still need to read it from file...
        //perhaps find a way to verify this number somehow, if the possibility exists of course...
        m_total_pop = p_tree.get<unsigned int>("popgen.pop_info.pop_total");

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

    void GeoGrid::generate_all() {
        generate_schools();
        generate_colleges();
        generate_workplaces();
        generate_communities();

    }

    void GeoGrid::generate_schools() {
        cout << m_schooled_frac << endl;
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
            auto c_schooled_pop = (unsigned int) round(it.second->getPopulation() * m_schooled_frac);
            pop_id.insert(pop_id.end(), c_schooled_pop, (const unsigned int &) it.first);
        }
        //Note that this way cuz of rounding we lose a couple of schooled ppl.
        //But this shouldn't affect our city divison.


        //Setting up random engine, TODO refactor to somewhere global
        random_device rand_dev;
        mt19937 gen (rand_dev()); //TODO need to remember state of this prob should be selectable.
        uniform_int_distribution<int>  distr(0, (unsigned int) pop_id.size() - 1);

        // assign schools to cities according to our normal distribution
        for(unsigned int i = 0; i < amount_of_schools; i++){
            school_count++;
            int index = pop_id[distr(gen)];
            shared_ptr<City> chosen_city = m_cities[index];
            shared_ptr<Community> nw_school(new Community(CommunityType::School, chosen_city));
            chosen_city->addCommunity(nw_school);
            //m_communities[nw_school->getID()] = nw_school
        }


    }

    unsigned int GeoGrid::findSmallest(const vector <shared_ptr<City>> &lc) {
        unsigned int smallest = 0;
        for (unsigned int i = 1; i < lc.size(); i++) {
            if (lc[smallest]->getPopulation() > lc[i]->getPopulation()) smallest = i;
        }
        return smallest;
    }

    void GeoGrid::adjustLargestCities(vector <shared_ptr<City>> &lc, const shared_ptr <City> &city) {
        if (lc.size() < m_maxlc) lc.push_back(city);
        else {
            unsigned int citpop = city->getPopulation();
            unsigned int smallest = findSmallest(lc);
            if (citpop > lc[smallest]->getPopulation()) lc[smallest] = city;
        }
    }

    void GeoGrid::generate_colleges() {
        //need 10 largest cities, largest determined by number of people in the city...
        vector <shared_ptr<City>> lc;
        for (auto &it : m_cities) {
            adjustLargestCities(lc, it.second);
        }

        //generate colleges to the respective cities...
        for (auto &it : lc) {
            //just checking which cities we found...
            //cout << it->getId() << "   " << it->getPopulation() << "   " << it->getName() << endl;

            //so let's go...
            double students = it->getPopulation()*m_workers1_frac*m_student_frac;
            //doesn't matter if students is a double at this time
            // since this is only an estimate for the number of colleges
            unsigned int nrcolleges =  round(students / m_college_size);
            //is this how we need to calculate the nr of colleges?
            // or did i not understand it properly?
            //cout << students << " students,   " << nrcolleges << " colleges for " << it->getName() << endl;

            for(unsigned int i = 0; i < nrcolleges; i++) {
                shared_ptr<Community> college = make_shared<Community>(CommunityType::College, it);
                it->addCommunity( college );
                //m_communities[college->getID()] = college
            }
        }
    }

    unsigned int GeoGrid::count_number_of_in_commuters(unsigned int destination_id) {
        unsigned int result = 0;

        for(auto it:m_commuting){
            result += it.second[destination_id];
        }
        return result;
    }

    unsigned int GeoGrid::count_number_of_out_commuters(unsigned int origin_id){
        unsigned int result = 0;
        map<unsigned int, unsigned int> destinations = m_commuting[origin_id];

        for(auto destination: destinations){
            if(destination.first != origin_id )
            result += destination.second;
        }

        return result;
    }

    void GeoGrid::setNumberOfCommuters(unsigned int origin_id, unsigned int destination_id, unsigned numberOfCommuters){
        m_commuting[origin_id][destination_id] = numberOfCommuters;
    }
    void GeoGrid::generate_workplaces() {
        //calculating the required informations

        //double working_population = m_active_frac * m_total_pop;
        //double number_of_workplaces = working_population/m_worksplace_size;

        //dividing workplaces to cities
        for (auto it:m_cities){
            shared_ptr<City> city = it.second;
            unsigned int in_commuters = this->count_number_of_in_commuters(it.first);
            //unsigned int out_commuters = this->count_number_of_out_commuters(it.first);

            //To be confirmed: everybody commutes, the in-commuters have all the people working in that region,
            //including locals who work in their own region
            //some percentages of the commuters are students
            double working_commuters = m_commuting_workers_frac * in_commuters;
            unsigned int number_of_workplaces = round(working_commuters / m_worksplace_size);

            for(unsigned int i=0; i<number_of_workplaces; i++){
                shared_ptr<Community> community = make_shared<Community>(CommunityType::Work, city);
                city->addCommunity(community);
            }

        }

    }

    /// Communities need to be distributed according to the relative population size.
    /// On average a community has 2000 members.
    void GeoGrid::generate_communities() {
        cout<<"start generating communities"<<endl;
        vector<shared_ptr<Community>> primsec_communities;
        /// First we need to determine the total number of communities to be used.
        auto total_communities = ceil(m_total_pop/m_community_size);
        /// Determine how many communities a city should get.
        for (auto it : m_cities){
            /// it: pair<int, City>
            shared_ptr<City> city = it.second;
            cout<<"city found"<<endl;
            /// ratio: the city contains ratio % of the total population.
            double ratio = (city->getPopulation()/(double)m_total_pop);
            cout<<city->getPopulation()<<" / "<<(double)m_total_pop<<endl;
            assert(0<ratio<1);
            /// Now we have the ratio, we know that the city has ratio % of all communities.
            auto city_communities = (total_communities*ratio);
            assert(city_communities<total_communities);

            for (int i = 0; i < city_communities; i++){
                /// Since there currently is no real difference between primary and secundary communities we make them all primary.
                shared_ptr<Community> community = make_shared<Community>(CommunityType::Primary, city);
                city->addCommunity(community);
                //m_communities[community->getID()] = community
            }
        }
    }


    unsigned int GeoGrid::count_total_pop() const {

        unsigned int counter = 0;
        for (auto &it : m_cities) {
            counter += it.second->getPopulation();
        }
        return counter;
    }

    const map<int, shared_ptr<City>>& GeoGrid::get_cities(){
        return m_cities;
    }

    unsigned int GeoGrid::getSchool_count() const {
        return school_count;
    };

}//namespace geogen