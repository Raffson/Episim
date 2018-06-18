//
// Created by robbe on 5/03/18.
//

#include <iostream>

#include "popgen-Episim/generator/GeoGridGenerator.h"
#include "popgen-Episim/generator/PopulationGenerator.h"
#include "popgen-Episim/util/GeoGridFileWriter.h"
#include "popgen-Episim/util/TestSummarizer.h"
#include "viewers/MapViewer.h"

using namespace std;

int main(int argc, char** argv)
{
        shared_ptr<stride::GeoGrid> grid = stride::GeoGridGenerator().Generate("run_default.xml", true);
        stride::GeoGridFileWriter::WriteAll(*grid);
        stride::PopulationGenerator(*grid).Generate();

        //This can be uncommented to generate html summary of all the testcases
        //stride::TestSummarizer summarizer;
        //summarizer.GenerateHtml("all_tests_episim.xml", "../../testsummary.html");

#ifdef USING_QT
        string outputPrefix = grid->GetConfigPtree().get<string>("run.output_prefix", "");
        const auto v = make_shared<stride::viewers::MapViewer>(grid, outputPrefix);
        v->LoadMap();
#endif
}
