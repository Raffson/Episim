//
// Created by Nishchal Shrestha on 16/06/2018.
//

#include "popgen-Episim/generator/GeoGridGenerator.h"
#include "popgen-Episim/generator/PopulationGenerator.h"
#include "gui/QTPopulation.h"

#include <map>
#include <iostream>
#include <QApplication>


void managePop(std::map<std::string, unsigned int>& popCounter, shared_ptr<stride::Population> population)
{
    unsigned int schooled = 0;
    unsigned int youngsters = 0;
    unsigned int middleAged = 0;
    unsigned int toddlers = 0;
    unsigned int oldies = 0;
    unsigned int active = 0;
    unsigned int unemployed = 0;
    unsigned int colleged = 0;

    for (unsigned int i = 0; i < population->size(); i++) {
        auto aPerson = population->at(i);

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
    popCounter["toddlers"] = toddlers;
    popCounter["schooled"] = schooled;
    popCounter["colleged"] = colleged;
    popCounter["youngsters"] = youngsters;
    popCounter["middleAged"] = middleAged;
    popCounter["oldies"] = oldies;
    popCounter["actives"] = active;
    popCounter["unemployed"] = unemployed;
}

void manageHouseholdAndWorkplaces(std::map<unsigned int, unsigned int>& householdCounter,
              std::map<std::string, unsigned int>& workplaceCounter, std::map<unsigned int, stride::City> cities)
{

    unsigned int micro = 0;
    unsigned int small = 0;
    unsigned int medium = 0;
    unsigned int large = 0;

    for(auto& aCity:cities){
        for(auto& hh:aCity.second.GetHouseholds()){
            unsigned int size =hh.GetMembers().size();
            if(householdCounter.find(size) == householdCounter.end()){
                householdCounter[size] = 1;
            }
            else{
                householdCounter[size] = householdCounter[size] + 1;
            }
        }

        for(auto& wp: aCity.second.GetWorkplaces()){
            unsigned int size = wp->GetSize();
            if(size <= 9){
                micro++;
            }
            else if(size > 9 && size <= 49){
                small++;
            }
            else if(size > 49 && size <= 249){
                medium++;
            }
            else{
                large++;
            }
        }
    }

    workplaceCounter["micro(1-9)"] = micro;
    workplaceCounter["small(10-49)"] = small;
    workplaceCounter["medium(50-249)"] = medium;
    workplaceCounter["large(250+)"] = large;

}

int main(int argc, char* argv[])
{

    shared_ptr<stride::GeoGrid> grid = stride::GeoGridGenerator().Generate("run_default.xml", true);
    stride::PopulationGenerator(*grid).Generate();

    std::map<std::string, unsigned int> popCounter;
    std::map<std::string, unsigned int> workplaceCounter;
    std::map<unsigned int, unsigned int> householdCounter;

    managePop(popCounter, grid->GetPopulation());
    manageHouseholdAndWorkplaces(householdCounter, workplaceCounter, grid->GetCities());


#ifdef USING_QT
    QApplication app(argc, argv);
    QTPopulation qtPopulation(popCounter, householdCounter, workplaceCounter);
    qtPopulation.VisualiseAll();
    app.exec();
#endif

}
