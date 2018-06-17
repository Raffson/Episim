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
class QObject;

using namespace std;

namespace stride {
namespace viewers {
        class MapViewer {
        public:
            MapViewer(shared_ptr<GeoGrid> grid, const std::string &outputPrefix = "");

            MapViewer(const MapViewer&)=delete;
            MapViewer& operator=(const MapViewer&)=delete;

            void LoadMap(bool showMap=true);

            /// Let viewer perform update.
            void Update(sim_event::Id id);

        private:
            void ToPng();

            shared_ptr<QQmlApplicationEngine> engine;
            shared_ptr<GeoGrid> m_grid;
            string m_output_prefix;
            unsigned int m_step;
            string m_map_option;
            string m_png_option;
            QObject* m_item;
        };
}
}

#endif //USING_QT

