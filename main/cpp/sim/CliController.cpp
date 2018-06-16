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
#include "util/ConfigInfo.h"
#include "util/FileSys.h"
#include "util/LogUtils.h"
#include "util/TimeStamp.h"
#include "viewers/AdoptedViewer.h"
#include "viewers/CliViewer.h"
#include "viewers/InfectedViewer.h"
#include "viewers/PersonsViewer.h"
#include "viewers/SummaryViewer.h"

#ifdef USING_QT
#include "viewers/MapViewer.h"
#endif

#include "boost/property_tree/xml_parser.hpp"

using namespace std;
using namespace stride::util;
using namespace boost::filesystem;
using namespace boost::property_tree;
using namespace boost::property_tree::xml_parser;

namespace stride {

CliController::CliController()
    : m_config_pt(), m_output_prefix(""), m_run_clock("run"), m_stride_logger(nullptr), m_use_install_dirs(),
      m_geogrid(nullptr), m_draw_map(false)
{
}

CliController::CliController(const ptree& configPt) : CliController()
{
        m_run_clock.Start();
        m_config_pt        = configPt;
        m_output_prefix    = m_config_pt.get<string>("run.output_prefix");
        m_use_install_dirs = m_config_pt.get<bool>("run.use_install_dirs");
        m_draw_map         = (m_config_pt.get("run.map_option", "none") != "none")
                             or (m_config_pt.get("run.png_option", "none") != "none");

        CheckEnv();
        CheckOutputPrefix();
        MakeLogger();
        LogSetup();
}

void CliController::CheckEnv()
{
        if (m_use_install_dirs) {
                auto log = [](const string& s) -> void { cerr << s << endl; };
                if (!FileSys::CheckInstallEnv(log)) {
                        throw runtime_error("CliController::CheckEnv> Install dirs not OK.");
                }
        }
}

void CliController::CheckOutputPrefix()
{
        if (FileSys::IsDirectoryString(m_output_prefix)) {
                try {
                        create_directories(m_output_prefix);
                } catch (exception& e) {
                        cerr << "CliController::Setup> Exception creating directory:  {}" << m_output_prefix << endl;
                        throw;
                }
        }
}

void CliController::Control()
{
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
        m_stride_logger->info("CliController shutting down.");
        spdlog::drop_all();
}

void CliController::ControlGui()
{
#ifdef USING_QT
        int dummyArgc = 0;
        QGuiApplication app(dummyArgc, 0); // main app
        QQmlApplicationEngine engine;
        QScopedPointer<gui::QTBackEnd> backend(new gui::QTBackEnd(engine, m_config_pt, this, nullptr));
        engine.rootContext()->setContextProperty("backend", backend.data());
        engine.load(QStringLiteral("mapviewer/Gui.qml"));
        app.exec();
#else
        cout << "Qt was not found, unable to start GUI..." << endl;
#endif
        m_stride_logger->info("CliController shutting down.");
        spdlog::drop_all();
}

void CliController::MakeLogger()
{
        const auto path     = FileSys::BuildPath(m_output_prefix, "stride_log.txt");
        const auto logLevel = m_config_pt.get<string>("run.stride_log_level");
        m_stride_logger     = LogUtils::CreateCliLogger("stride_logger", path.string());
        m_stride_logger->set_level(spdlog::level::from_str(logLevel));
        m_stride_logger->flush_on(spdlog::level::err);
}

void CliController::RegisterViewers(shared_ptr<SimRunner> runner)
{
        // Command line viewer
        m_stride_logger->info("Registering CliViewer");
        const auto cli_v = make_shared<viewers::CliViewer>(runner, m_stride_logger);
        runner->Register(cli_v, bind(&viewers::CliViewer::Update, cli_v, placeholders::_1));

        // Adopted viewer
        if (m_config_pt.get<bool>("run.output_adopted", false)) {
                m_stride_logger->info("registering AdoptedViewer,");
                const auto v = make_shared<viewers::AdoptedViewer>(runner, m_output_prefix);
                runner->Register(v, bind(&viewers::AdoptedViewer::Update, v, placeholders::_1));
        }

        // Infection counts viewer
        if (m_config_pt.get<bool>("run.output_cases", false)) {
                m_stride_logger->info("Registering InfectedViewer");
                const auto v = make_shared<viewers::InfectedViewer>(runner, m_output_prefix);
                runner->Register(v, bind(&viewers::InfectedViewer::Update, v, placeholders::_1));
        }

        // Persons viewer
        if (m_config_pt.get<bool>("run.output_persons", false)) {
                m_stride_logger->info("registering PersonsViewer.");
                const auto v = make_shared<viewers::PersonsViewer>(runner, m_output_prefix);
                runner->Register(v, bind(&viewers::PersonsViewer::Update, v, placeholders::_1));
        }

        // Summary viewer
        if (m_config_pt.get<bool>("run.output_summary", false)) {
                m_stride_logger->info("Registering SummaryViewer");
                const auto v = make_shared<viewers::SummaryViewer>(runner, m_output_prefix);
                runner->Register(v, bind(&viewers::SummaryViewer::Update, v, placeholders::_1));
        }


#ifdef USING_QT
        // Map viewer, m_geogrid should never be null but checking just in case...
        if (m_draw_map and m_geogrid) {
            m_stride_logger->info("Registering MapViewer");
            const auto v = make_shared<viewers::MapViewer>(m_geogrid, m_output_prefix);
            runner->Register(v, bind(&viewers::MapViewer::Update, v, placeholders::_1));
        }
#endif
}

void CliController::LogSetup()
{
        m_stride_logger->info("CliController stating up at: {}", TimeStamp().ToString());
        m_stride_logger->info("Executing revision {}", ConfigInfo::GitRevision());
        m_stride_logger->info("Creating dir:  {}", m_output_prefix);
        m_stride_logger->trace("Executing:           {}", FileSys::GetExecPath().string());
        m_stride_logger->trace("Current directory:   {}", FileSys::GetCurrentDir().string());
        if (m_use_install_dirs) {
                m_stride_logger->trace("Install directory:   {}", FileSys::GetRootDir().string());
                m_stride_logger->trace("Config  directory:   {}", FileSys::GetConfigDir().string());
                m_stride_logger->trace("Data    directory:   {}", FileSys::GetDataDir().string());
        }
        if (ConfigInfo::HaveOpenMP()) {
                m_stride_logger->info("Max number OpenMP threads in this environment: {}",
                                      ConfigInfo::NumberAvailableThreads());
                m_stride_logger->info("Configured number of threads: {}",
                                      m_config_pt.get<unsigned int>("run.num_threads"));
        } else {
                m_stride_logger->info("Not using OpenMP threads.");
        }
}

} // namespace stride
