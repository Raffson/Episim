#pragma once
//
// Created by Raphael A. on 18/05/2018.
//

#include "popgen-Episim/model/GeoGrid.h"

#include <boost/filesystem.hpp>


namespace stride {

class GeoGridGenerator
{

public:
        /// Default constructor
        GeoGridGenerator() : m_grid(nullptr), m_cid_generator(1) {};

        /// Generates a GeoGrid according to the given configuration file.
        /// If the file is not found, 'run_default.xml' will be chosen loaded by default.
        /// @param config: A path to a geogen config file. This file contains
        ///             things like name of the city data file, information about
        ///             the population...
        /// @param contactFile: A boolean value indicating if we're overriding the 'output_contact_file' value
        /// of the given configuration. This is specifically to prevent output during the tests.
        /// @retval <std::shared_ptr<GeoGrid>> A shared pointer to the generated GeoGrid.
        std::shared_ptr<GeoGrid> Generate(const boost::filesystem::path& config, const bool contactFile = false);


        /// Generates a GeoGrid according to the given property tree which contains the configuration.
        /// @param pTree The property tree containing the configuration for the GeoGrid generation.
        /// @retval <std::shared_ptr<GeoGrid>> A shared pointer to the generated GeoGrid.
        std::shared_ptr<GeoGrid> Generate(const boost::property_tree::ptree& pTree);

private:
        /// Assigns the main fractions: schooled, worker1, worker2 & rest
        void GetMainFractions();

        /// Used by GeoGridGenerator::Generate to create an output-directory if needed.
        void InitOutputStuff();

        /// Used by GeoGridGenerator::Generate to add the Popgen's configuration ptree.
        void AddPopgenPtree();

        /// Used by GeoGridGenerator::Generate to read the Fractions and Sizes.
        void ReadFractionsAndSizes();

        /// Used by GeoGridGenerator::Generate to read cities, households & commuting data from files.
        void ReadDataFiles();

        /// Used by GeoGridGenerator::Generate to ensure a consistent configuration was given before generating
        /// communities and their contact pools.
        void EnsureConsistency();

        /// Reads and builds communities from a file.
        /// @param fname The name of the file to be read.
        void CommunitiesFromFile(const string& fname = "communities.csv");

        /// Reads and builds a population for the GeoGrid from a file.
        /// @param fname The name of the file to be read.
        void PopulationFromFile(const string& fname = "population.csv");

        /// Calls all functions relating to community/contactpool generation and neighbour-classification.
        void GenerateAll();

        /// Generates the schools, places them into the cities using a discrete distribution.
        /// Preconditions:\n
        ///     \li REQUIRE(m_schooled_frac <= 1, "Schooled Fract is bigger then 1, not possible!");\n
        ///     \li REQUIRE(m_schooled_frac >= 0, "Schooled fract can't be negative");\n
        ///     \li REQUIRE(m_school_size >= 0, "The initial school size can't be negative");
        ///
        /// Postconditions:\n
        ///     \li ENSURE(Schools are placed in cities according to discrete distribution) -> enforced in test enviroment
        void GenerateSchools();

        /// Returns index of city with smallest population from 'lc'
        /// used by AdjustLargestCities(lc, city)
        unsigned int FindSmallest(const vector<City*>& lc);

        /// Adjusts 'lc' iff 'city' has more people than the city with the smallest population in 'lc'
        /// used by GenerateColleges()
        void AdjustLargestCities(vector<City*>& lc, City& city);

        /// Generates the colleges, places them into the cities
        /// using a discrete distribution.
        /// Preconditions:\n
        ///     \li REQUIRE(m_student_frac >= 0, "Student fractal can't be negative");\n
        ///     \li REQUIRE(m_student_frac <= 1, "Student fractal can't be more then 100%");\n
        ///     \li REQUIRE(m_workers1_frac >= 0, "Worker fractal can't be negative");\n
        ///     \li REQUIRE(m_workers1_frac <= 1, "Worker fractal can't be more then 100%");
        ///
        /// Postconditions:\n
        ///     \li ENSURE(colleges are placed in x biggest cities) -> enforced in test envirorement
        //          Need to enforce this ensure in the code as well...
        void GenerateColleges();

        /// Generates the workplaces, places them into the cities a random with chances
        /// discretly distributed according to size. Generates contactpools for the communities.
        void GenerateWorkplaces();

        /// Generates the communties, places them into the cities a random with chances
        /// discretly distributed according to size. Generates contactpools for the communities.
        void GenerateCommunities();

        /// Computes for each city the distances to all other cities and classifies them
        /// in exponential order, assigning this to m_neighbours_in_radius. The default initial search radius = 10km.
        void ClassifyNeighbours();
        void ClassifyNeighbours2(); //for benchmarking purposes...

        /// Adds communities of the given type and their contact pools.
        /// @param cities A vector with pointers to all the cities in the GeoGrid.
        /// @param indices A vector with random indices for 'cities'.
        /// @param type The type of the communities to be added.
        void AddCommunities(const vector<City*>& cities, const vector<unsigned int>& indices, CommunityType::Id type);

private:
        std::shared_ptr<GeoGrid> m_grid;          ///< The GeoGrid object.
        size_t                   m_cid_generator; ///< ID generator for communities

};

} //namespace stride