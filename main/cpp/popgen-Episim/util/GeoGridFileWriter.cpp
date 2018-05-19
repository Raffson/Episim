//
// Created by Raphael A. on 17/05/2018.
//

#include "GeoGridFileWriter.h"
#include "popgen-Episim/model/GeoGrid.h"
#include "util/FileSys.h"

#include <fstream>

using namespace std;

namespace stride {

void GeoGridFileWriter::WriteAll(const GeoGrid &grid) {
    //creating a directory if it doesn't exist
    std::string path = grid.m_config_pt.get<string>("run.output_prefix");
    path += util::FileSys::IsDirectoryString(path) ? "GeoGrid/" : "/GeoGrid/";
    boost::filesystem::path dir(path.c_str());
    if (!(boost::filesystem::exists(dir))) {
        boost::filesystem::create_directory(dir);
    }

    WriteModels(grid, dir.string());
    WritePopulation(grid, dir.string() + "population.csv");
    WriteCommunities(grid, dir.string() + "communities.csv");
    WriteRNGstate(grid, dir.string() + "RNG-state.xml");

    boost::property_tree::ptree pt = grid.m_config_pt;
    pt.get_child("run").erase("popgen");
    pt.put("run.geopop_file", dir.string() + "geogen.xml");
    pt.put("run.population_file", dir.string() + "population.csv");
    pt.put("run.communities_file", dir.string() + "communities.csv");
    pt.put("run.rng_state_file", dir.string() + "RNG-state.xml");
    pt.put("run.prebuilt_geopop", true);
    pt.put("run.use_install_dirs", false);
    pt.put("run.output_prefix", "");
    pt.get_child("run").sort();
    ptreeToFile(pt, dir.string() + "config.xml");

    pt = grid.m_config_pt.get_child("run.popgen");
    pt.put("data_files.cities", dir.string() + "cities.csv");
    pt.put("data_files.commuting", dir.string() + "commuting.csv");
    pt.put("data_files.households", dir.string() + "households.xml");
    pt.get_child("data_files").sort();
    pt.put("neighbour_classification.initial_search_radius", grid.m_initial_search_radius);
    pt.put("pop_info.pop_total", grid.m_total_pop);
    pt.put("pop_info.random_ages", grid.m_random_ages);
    pt.put("pop_info.fraction_students", grid.m_fract_map.at(Fractions::STUDENTS));
    pt.put("pop_info.fraction_commuting_students", grid.m_fract_map.at(Fractions::COMMUTING_STUDENTS));
    pt.put("pop_info.fraction_active_workers", grid.m_fract_map.at(Fractions::ACTIVE));
    pt.put("pop_info.fraction_commuting_workers", grid.m_fract_map.at(Fractions::COMMUTING_WORKERS));
    pt.put("contactpool_info.average_size", grid.m_sizes_map.at(Sizes::AVERAGE_CP));
    pt.put("contactpool_info.school.size", grid.m_sizes_map.at(Sizes::SCHOOLS));
    pt.put("contactpool_info.college.size", grid.m_sizes_map.at(Sizes::COLLEGES));
    pt.put("contactpool_info.college.cities", grid.m_sizes_map.at(Sizes::MAXLC));
    pt.get_child("contactpool_info.college").sort();
    pt.put("contactpool_info.community.size", grid.m_sizes_map.at(Sizes::COMMUNITIES));
    pt.put("contactpool_info.workplace.size", grid.m_sizes_map.at(Sizes::WORKPLACES));
    pt.get_child("contactpool_info").sort();
    pt.sort();
    ptreeToFile(pt, dir.string() + "geogen.xml");
}

void GeoGridFileWriter::WriteModels(const GeoGrid &grid, const string &path) {
    WriteCities(grid, path + "cities.csv");
    WriteCommuting(grid, path + "commuting.csv");
    WriteModelHouseholds(grid, path + "households.xml");
}

void GeoGridFileWriter::WriteCities(const GeoGrid &grid, const string &fname) {
    std::ofstream file;
    file.open(fname.c_str(), std::ofstream::out);
    file << R"("id","province","population","x_coord","y_coord","latitude","longitude", "name")" << endl;
    for (const auto &city : grid.m_cities) {
        const City &c = city.second;
        const Coordinate &co = c.GetCoordinates();
        file << c.GetId() << "," << c.GetProvince() << "," << c.GetPopulation() << "," << co.GetX() << ","
             << co.GetY() << "," << co.GetLatitude() << "," << co.GetLongitude() << "," << c.GetName() << endl;
    }
    file.close();
}

void GeoGridFileWriter::WriteCommuting(const GeoGrid &grid, const string &fname) {
    std::ofstream file;
    file.open(fname.c_str(), std::ofstream::out);
    for (const auto &city : grid.m_cities) {
        file << "\"id_" << city.second.GetId() << "\"";
        if (city.first != grid.m_cities.rbegin()->first) file << ",";
    }
    file << endl;
    for (const auto &city : grid.m_cities) {
        const City &c = city.second;
        for (const auto &incomm : c.GetInCommuting()) {
            file << incomm.second;
            if (incomm.first != c.GetInCommuting().rbegin()->first) file << ",";
        }
        file << endl;
    }
    file.close();
}

void GeoGridFileWriter::WriteModelHouseholds(const GeoGrid &grid, const string &fname) {
    boost::property_tree::ptree tree;
    for (const auto &it: grid.m_model_households) {
        for (auto &hh: it.second) {
            boost::property_tree::ptree t2;
            for (auto &age: hh) {
                t2.add("age", age);
            }
            tree.add_child("HouseholdProfile.hh", t2);
        }
    }
    ptreeToFile(tree, fname);
}

void GeoGridFileWriter::WritePopulation(const GeoGrid &grid, const string &fname) {
    if (grid.m_population) {
        std::ofstream file;
        file.open(fname.c_str(), std::ofstream::out);
        file << R"("age","household_id","school_id","work_id","primary_community","secundary_community","city_id")"
             << endl;
        for (const auto &pool : grid.m_population->GetContactPoolSys()[ContactPoolType::Id::Household]) {
            unsigned int cid = pool.GetHousehold()->GetCity()->GetId();
            for (const auto &p : pool.GetPool())
                file << *p << "," << cid << endl;
        }
        file.close();
    } else {
        cerr << "GeoGridFileWriter::WritePopulation> Can't write uninitialized population." << endl;
    }
}

void GeoGridFileWriter::WriteCommunities(const GeoGrid &grid, const string &fname) {
    if (grid.m_population) {
        std::ofstream file;
        file.open(fname.c_str(), std::ofstream::out);
        file << R"("community_id","community_type","city_id")" << endl;
        const ContactPoolSys &cps = grid.m_population->GetContactPoolSys();
        for (const auto &id : ContactPoolType::IdList) {
            if (id == ContactPoolType::Id::Household)
                continue; //cause we'll build households when reading the population
            for (unsigned int i = 0; i < cps[id].size();) {
                const ContactPool &cp = cps[id][i];
                file << cp.GetCommunity()->GetID()
                     << "," << CommunityType::ToString(cp.GetCommunity()->GetCommunityType())
                     << "," << cp.GetCommunity()->GetCity()->GetId() << endl;
                Sizes size = CommunityType::ToSizes(cp.GetCommunity()->GetCommunityType());
                i += (unsigned int) ceil((double) grid.m_sizes_map.at(size) / grid.m_sizes_map.at(Sizes::AVERAGE_CP));
            }
        }
        file.close();
    } else {
        cerr << "GeoGridFileWriter::WriteCommunities> Can't write with an uninitialized contact pool system."
             << endl;
    }
}

void GeoGridFileWriter::WriteRNGstate(const GeoGrid &grid, const string &fname) {
    boost::property_tree::ptree pt;
    pt.put("rng_state.seed", grid.m_rng.GetInfo().m_seed);
    pt.put("rng_state.state", grid.m_rng.GetInfo().m_state);
    pt.put("rng_state.stream_count", grid.m_rng.GetInfo().m_stream_count);
    pt.put("rng_state.type", grid.m_rng.GetInfo().m_type);

    ptreeToFile(pt, fname);
}

}
