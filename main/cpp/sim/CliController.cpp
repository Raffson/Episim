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
 * Header for the command line controller.
 */

#include "sim/CliController.h"

#include "pop/Population.h"
#include "popgen-Episim/generator/GeoGridGenerator.h"
#include "popgen-Episim/generator/PopulationGenerator.h"
#include "sim/SimRunner.h"

#ifdef USING_QT
#include "viewers/MapViewer.h"
#endif

#include <boost/property_tree/ptree.hpp>

using namespace std;
using namespace stride::util;
using namespace boost::property_tree;

namespace stride {

CliController::CliController(const ptree& configPt)
        : ControlHelper("CliController", configPt), m_geogrid(nullptr), m_draw_map(false) {}

void CliController::Control()
{
        // -----------------------------------------------------------------------------------------
        // Prelims.
        // -----------------------------------------------------------------------------------------
        m_draw_map         = (m_config_pt.get("run.map_option", "none") != "none")
                             or (m_config_pt.get("run.png_option", "none") != "none");
        CheckEnv();
        CheckOutputPrefix();
        InstallLogger();
        LogStartup();

        // -----------------------------------------------------------------------------------------
        // Build population, instantiate SimRunner & register viewers & run.
        // -----------------------------------------------------------------------------------------
        std::shared_ptr<Population> pop;
        if( m_config_pt.get<bool>("run.random_geopop", false) ) {
            m_geogrid = GeoGridGenerator().Generate(m_config_pt);
            PopulationGenerator(*m_geogrid).Generate();
            pop = m_geogrid->GetPopulation();
        }
        else pop = Population::Create(m_config_pt);
        auto runner = make_shared<SimRunner>(m_config_pt, pop, m_geogrid);
        RegisterViewers(runner);
        runner->Run();

        // -----------------------------------------------------------------------------------------
        // Shutdown.
        // -----------------------------------------------------------------------------------------
        LogShutdown();
        spdlog::drop_all();
}

void CliController::ControlGui()
{

#ifdef USING_QT
        CheckEnv();
        CheckOutputPrefix();
        InstallLogger();
        LogStartup();

        int dummyArgc = 0;
        QGuiApplication app(dummyArgc, nullptr); // main app
        QQmlApplicationEngine engine;
        QScopedPointer<gui::QTBackEnd> backend(new gui::QTBackEnd(engine, m_config_pt, this, nullptr));
        engine.rootContext()->setContextProperty("backend", backend.data());
        engine.load(QStringLiteral("mapviewer/Gui.qml"));
        app.exec();

        LogShutdown();
        spdlog::drop_all();
#else
        cout << "Qt was not found, unable to start GUI..." << endl;
#endif
}

void CliController::RegisterViewers(shared_ptr<SimRunner> runner)
{
        this->ControlHelper::RegisterViewers(runner);

#ifdef USING_QT
        // Map viewer, m_geogrid should never be null but checking just in case...
        if (m_draw_map and m_geogrid) {
            m_stride_logger->info("Registering MapViewer");
            const auto v = make_shared<viewers::MapViewer>(m_geogrid, m_output_prefix);
            runner->Register(v, bind(&viewers::MapViewer::Update, v, placeholders::_1));
        }
#endif
}

} // namespace stride
