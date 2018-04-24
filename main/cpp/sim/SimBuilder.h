#pragma once
/*
 *  This is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  The software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with the software. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2017, 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Header for the SimulatorBuilder class.
 */

#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <spdlog/spdlog.h>

#include "popgen-Episim/GeoGen/GeoGrid.h"
#include "popgen-Episim/PopGen/PopulationGenerator.h"

namespace stride {

class Sim;

/**
 * Builds a simulator (@see Sim) based a configuration property tree.
 * It
 * \li reads any additional configuration files (disease, contact, ...)
 * \li initializes calendar and random number manager for the simulator
 * \li builds a population and contact pool system (@see PopulationBuilder)
 * \li deals with health and immunity (@see HealthSeeder, @see DiseaseBuilder)
 */
class SimBuilder
{
public:
        /// Initializing SimulatorBuilder.
        SimBuilder(const boost::property_tree::ptree& configPt, std::shared_ptr<spdlog::logger> logger);

        /// Build the simulator.
        std::shared_ptr<Sim> Build(std::shared_ptr<GeoGrid>& grid);

private:
        /// Build the simulator.
        std::shared_ptr<Sim> Build(const boost::property_tree::ptree& diseasePt,
                                         const boost::property_tree::ptree& ageContactPt,
                                         std::shared_ptr<GeoGrid>& grid);

        /// Get the contact configuration data.
        boost::property_tree::ptree ReadAgeContactPtree();

        /// Get the disease configuration data.
        boost::property_tree::ptree ReadDiseasePtree();

private:
        boost::property_tree::ptree     m_config_pt;     ///< Run config in ptree.
        std::shared_ptr<spdlog::logger> m_stride_logger; ///< Stride run logger.
};

} // namespace stride
