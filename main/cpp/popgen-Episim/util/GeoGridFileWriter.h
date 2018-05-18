#pragma once
//
// Created by Raphael A. on 17/05/2018.
//

#include <string>

namespace stride {

class GeoGrid;

/// This class is purely a collection of static functions.
/// No construction of this object is allowed.
class GeoGridFileWriter {

public:

    /// No construction allowed.
    GeoGridFileWriter() = delete;

    /// Write the entire GeoGrid to files. The configuration will be written along with the cities,
    /// household model, commuting model, contact pool system and population.
    /// @param grid The GeoGrid object to be written.
    static void WriteAll(const GeoGrid &grid);

    /// Writes the current models to files. All cities, commuting data and households will be written.
    /// @param grid The GeoGrid object from which the models (cities, commuting and household models) will be written.
    /// @param path The working directory in which the files should be written. Default is the path of execution.
    static void WriteModels(const GeoGrid &grid, const std::string &path = "");

    /// Writes the current cities to a file.
    /// @param grid The GeoGrid object from which the cities will be written.
    /// @param fname The name of the file to be written.
    static void WriteCities(const GeoGrid &grid, const std::string &fname = "cities.csv");

    /// Writes the current commuting data to a file.
    /// @param grid The GeoGrid object from which the commuting data will be written.
    /// @param fname The name of the file to be written.
    static void WriteCommuting(const GeoGrid &grid, const std::string &fname = "commuting.csv");

    /// Writes the current model of households to a file.
    /// @param grid The GeoGrid object from which the household model will be written.
    /// @param fname The name of the file to be written.
    static void WriteModelHouseholds(const GeoGrid &grid, const std::string &fname = "households.xml");

    /// Writes the current population to a file.
    /// @param grid The GeoGrid object from which the population will be written.
    /// @param fname The name of the file to be written.
    static void WritePopulation(const GeoGrid &grid, const std::string &fname = "population.csv");

    /// Writes the current contact pool system to a file.
    /// @param grid The GeoGrid object from which the communities will be written.
    /// @param fname The name of the file to be written.
    static void WriteCommunities(const GeoGrid &grid, const std::string &fname = "communities.csv");

    /// Writes the current RNG's state to a file.
    /// @param grid The GeoGrid object from which the RNG-state will be written.
    /// @param fname The name of the file to be written.
    static void WriteRNGstate(const GeoGrid &grid, const std::string &fname = "RNG-state.xml");

};

}

