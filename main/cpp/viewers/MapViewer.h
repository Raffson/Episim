#pragma once
//
// Created by beau on 5/8/18.
//
#ifdef USING_QT


#include "popgen-Episim/model/GeoGrid.h"
#include "output/SummaryFile.h"
#include "sim/SimRunner.h"
#include "sim/event/Id.h"


using namespace std;

namespace stride {
namespace viewers {
        class MapViewer {
        public:
            MapViewer(shared_ptr<GeoGrid> grid, const std::string &outputPrefix = "");

            MapViewer(const MapViewer&)=delete;
            MapViewer& operator=(const MapViewer&)=delete;

            void LoadMap(bool showMap=true, bool makePng=false);

            /// Let viewer perform update.
            void Update(sim_event::Id id);

        private:
            void ToPng();

            shared_ptr<GeoGrid> m_grid;
            string m_output_prefix;
            unsigned int m_step;
            string m_map_option;
            string m_png_option;
        };
}
}

#endif //USING_QT

