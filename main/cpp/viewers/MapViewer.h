//
// Created by beau on 5/8/18.
//

#include <iostream>
#include <sstream>

#include "popgen-Episim/GeoGen/GeoGrid.h"
#include "popgen-Episim/PopGen/PopulationGenerator.h"
#include "output/SummaryFile.h"
#include "sim/SimRunner.h"
#include "sim/event/Id.h"

#include <spdlog/spdlog.h>

#ifdef USING_QT
#include <QPair>
#include <QString>
#include <QtCore/QTextStream>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQuick/QQuickItem>
#endif

using namespace std;

namespace stride {
    namespace viewers {
        class MapViewer {
        public:
            MapViewer(std::shared_ptr<SimRunner> runner, const std::string &output_prefix, shared_ptr<stride::GeoGrid> grid)
                    : m_summary_file(output_prefix), m_runner(std::move(runner)), m_grid(grid)
            {
                Initialize();
            }

            void Initialize();

            /// Let viewer perform update.
            void Update(sim_event::Id id);

            /// Load the cities from a geogrid.
            void loadCities();

        private:
            shared_ptr<QQmlApplicationEngine> engine;
            shared_ptr<stride::GeoGrid> m_grid;
            output::SummaryFile m_summary_file;
            std::shared_ptr<SimRunner> m_runner;
        };
    }
}

