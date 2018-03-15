//
// Created by beau on 3/5/18.
//

#include "GeoGrid.h"


using namespace std;

namespace geogen {

    GeoGrid::GeoGrid(const boost::filesystem::path & config_file) {

        //Setting up property tree to parse xml config file
        boost::property_tree::ptree p_tree;
        boost::property_tree::read_xml(config_file.string(), p_tree);

        //reading the cities data file
        boost::filesystem::path base_path = "data/";
        string city_file = p_tree.get("popgen.data_files.cities","flanders_cities.csv");
        m_cities = parser::parse_cities(base_path.append(city_file));

        string commuting_file =  p_tree.get("popgen.data_files.commuting","flanders_commuting.csv");
        m_commuting = parser::parse_commuting("data/" + commuting_file);

        //Generating schools
        //auto total_pop = p_tree.get<unsigned int>("popgen.pop_info.pop_total");
        //specs ask this to be read out of config, but could be calculated directly
        //out of the city file?
        // -> you're right... so let's do it like this:
        m_total_pop = count_total_pop();

        m_schooled_frac = p_tree.get<float>("popgen.pop_info.fraction_schooled");
        m_school_size = p_tree.get<unsigned int>("popgen.contactpool_info.school.size");
        generate_schools();

        m_student_frac = p_tree.get<float>("popgen.pop_info.fraction_student");
        m_college_size =  p_tree.get<unsigned int>("popgen.contactpool_info.college.size");
        generate_colleges();

        m_community_size_limit = p_tree.get<unsigned int>("popgen.contactpool_info.community.size");
        generate_communities();

        m_active_frac = p_tree.get<float>("popgen.pop_info.fraction_worker");
        m_worksplace_size = p_tree.get<unsigned int>("popgen.contactpool_info.workplace.size");

        m_commuting_workers = p_tree.get<float>("popgen.pop_info.fraction_commuting_workers");
        generate_workplaces();
    }

    void GeoGrid::generate_schools() {

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

            int index = pop_id[distr(gen)];
            shared_ptr<City> chosen_city = m_cities[index];
            shared_ptr<Community> nw_school(new Community(CommunityType::School, chosen_city));
            chosen_city->addCommunity(nw_school);
        }


    }

    unsigned int findSmallest(const vector <shared_ptr<City>> &lc) {
        unsigned int smallest = 0;
        for (unsigned int i = 1; i < lc.size(); i++) {
            if (lc[smallest]->getPopulation() > lc[i]->getPopulation()) smallest = i;
        }
        return smallest;
    }

    void adjustLargestCities(vector <shared_ptr<City>> &lc, const shared_ptr <City> &city, unsigned int maxlc) {
        if (lc.size() < maxlc) lc.push_back(city);
        else {
            unsigned int citpop = city->getPopulation();
            unsigned int smallest = findSmallest(lc);
            if (citpop > lc[smallest]->getPopulation()) lc[smallest] = city;
        }
    }

    void GeoGrid::generate_colleges(unsigned int maxlc) {
        //need 10 largest cities, largest determined by number of people in the city...
        vector <shared_ptr<City>> lc;
        for (auto &it : m_cities) {
            adjustLargestCities(lc, it.second, maxlc);
        }

        //generate colleges to the respective cities...
        for (auto &it : lc) {
            //just checking which cities we found...
            //cout << it->getId() << "   " << it->getPopulation() << "   " << it->getName() << endl;

            //so let's go...
            double students = it->getPopulation()*m_student_frac;
            //doesn't matter if students it's a double at this time
            // since this is only an estimate for the number of colleges
            unsigned int nrcolleges =  round(students / m_college_size);
            cout << students << " students,   " << nrcolleges << " colleges for " << it->getName() << endl;
            //now do we create a college and add it to the city's vector of colleges?
            //or do we create a college that references the city's  id?
            //need to discuss this with group to maintain a consistent way of working
            //at this point Robbe decided to the the first while Beau opted for the latter...
            //-> we need both!
        }
    }

    unsigned int GeoGrid::count_number_of_in_commuters(unsigned int destination_id) {
        unsigned int result = 0;
        for(auto it:m_commuting){
            if(it.first.second == destination_id){
                result += it.second;
            }
        }
        return result;
    }

    unsigned int GeoGrid::count_number_of_out_commuters(unsigned int origin_id){
        unsigned int result = 0;
        for(auto it:m_commuting){
            if(it.first.first == origin_id){
                //commuting in own region shouldn't be counted
                if(it.first.first != it.first.second){
                    result += it.second;

                }
            }
        }
        return result;
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
            double working_commuters = m_commuting_workers * in_commuters;
            unsigned int number_of_workplaces = round(working_commuters / m_worksplace_size);

            for(unsigned int i=0; i<number_of_workplaces; i++){
                shared_ptr<Community> community = make_shared<Community>(CommunityType::Work, city);
                city->addCommunity(community);
            }

        }

    }

    void GeoGrid::generate_communities() {
        vector<shared_ptr<Community>> primsec_communities;
        /// Communities need to be distributed according to the relative population size.
        /// First we need to determine the total number of communities to be used.
        auto total_communities = ceil(m_total_pop/m_community_size_limit);
        for (auto it : m_cities){
            shared_ptr<City> city = it.second;
            auto ratio = city->getPopulation()/m_total_pop;
            /// Now we have the ratio, we know that the city has ratio % of all communities.
            auto city_communities = (total_communities*ratio)/100;
            for (int i = 0; i < city_communities; i++){
                /// used primary communities atm since i have no clue what this has to be...
                shared_ptr<Community> community = make_shared<Community>(CommunityType::Primary, city);
                city->addCommunity(community);
            }
        }
        /// TODO: determine if community is primary or secundary.
    }


    unsigned int GeoGrid::count_total_pop() const {

        unsigned int counter = 0;
        for (auto &it : m_cities) {
            counter += it.second->getPopulation();
        }
        return counter;
    }

}//namespace geogen