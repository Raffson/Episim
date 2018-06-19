//
// Created by Nishchal Shrestha on 16/06/2018.
//

#include "popgen-Episim/generator/GeoGridGenerator.h"
#include "popgen-Episim/generator/PopulationGenerator.h"
#include "gui/QTPopulation.h"

#include <QApplication>

int main(int argc, char* argv[])
{

    shared_ptr<stride::GeoGrid> grid = stride::GeoGridGenerator().Generate("run_default.xml", true);
    stride::PopulationGenerator(*grid).Generate();

    unsigned int schooled = 0;
    unsigned int youngsters = 0;
    unsigned int middleAged = 0;
    unsigned int toddlers = 0;
    unsigned int oldies = 0;
    unsigned int active = 0;
    unsigned int unemployed = 0;
    unsigned int colleged = 0;

    for (unsigned int i = 0; i < grid->GetPopulation()->size(); i++) {
        auto aPerson = grid->GetPopulation()->at(i);

        switch (stride::get_category(aPerson.GetAge())) {
            case stride::Fractions::SCHOOLED : {
                schooled++;
                break;
            }
            case stride::Fractions::YOUNG : {
                youngsters++;
                if (aPerson.GetPoolId(stride::ContactPoolType::Id::Work) != 0) {
                    active++;
                }
                else if(aPerson.GetPoolId(stride::ContactPoolType::Id::School) != 0){
                    colleged++;
                }
                break;
            }
            case stride::Fractions::MIDDLE_AGED : {
                middleAged++;
                if (aPerson.GetPoolId(stride::ContactPoolType::Id::Work) != 0) {
                    active++;
                }
                else{
                    unemployed++;
                }
                break;
            }
            case stride::Fractions::TODDLERS : {
                toddlers++;
                break;
            }
            default: {
                oldies++;
                break;
            }
        }
    }
#ifdef USING_QT
    QApplication app(argc, argv);
    QTPopulation qtPopulation(toddlers, schooled, colleged, youngsters, middleAged, oldies, active, unemployed);
    qtPopulation.VisualiseAll();
    app.exec();
#endif

}
