#pragma once
//
// Created by beau on 5/8/18.
//
#ifdef USING_QT


#include "popgen-Episim/model/GeoGrid.h"
#include "output/SummaryFile.h"
#include "sim/SimRunner.h"
#include "sim/event/Id.h"

class QQmlApplicationEngine;

using namespace std;

namespace stride {
namespace viewers {
        class MapViewer {
        public:
            MapViewer(std::shared_ptr<SimRunner> runner, const std::string &output_prefix, shared_ptr<stride::GeoGrid> grid)
                    : m_grid(grid), m_summary_file(output_prefix), m_runner(std::move(runner))
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

#endif //USING_QT

