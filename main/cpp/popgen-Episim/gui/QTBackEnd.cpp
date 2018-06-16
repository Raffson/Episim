//
// Created by robbe on 03.06.18.
//

#include "QTBackEnd.h"
#include <QtQuick/QQuickView>
#include <string>
#include <util/FileSys.h>
#include "util/InstallDirs.h"
#include <boost/filesystem/path.hpp>
#include <sim/SimRunner.h>
#include <fstream>
using namespace std;

QTBackEnd::QTBackEnd(QQmlApplicationEngine& engine, ptree& pt, QObject *parent):QObject(parent),
                    m_pt(pt),m_engine(engine) {

    string file(m_pt.get<string>("run.geopop_file"));
    string path = "config/" + file;
    fstream filestr;
    filestr.open(path);
    read_xml(path, m_geo_pt);
    filestr.close();
}


void QTBackEnd::genPop() {


    string file(m_pt.get<string>("run.geopop_file"));
    string path = "config/" + file;
    fstream filestr;
    filestr.open(path);
    write_xml(path, m_geo_pt);
    filestr.close();

    this->m_grid = stride::GeoGridGenerator().Generate(m_pt);
    stride::PopulationGenerator(*m_grid).Generate();

    m_pop_generated = true;

    m_total_pop = m_grid->GetTotalPop();
    emit PopChanged();

    MakeCityList();
}

void QTBackEnd::MakeCityList() {


    m_cities.clear();
    m_commuters.clear();
    for(auto& it: m_grid->GetCities()){
        m_cities.append(new QTCity(&it.second, this));
    }
    for(auto& it: m_cities){ // Needs to happen when the full list is initialized
        auto cty = dynamic_cast<QTCity*>(it);

        cty->CreateCommutingList(10);
    }
    emit CitiesChanged();
}

QGeoCoordinate QTBackEnd::GetCenter() {

    stride::Coordinate crd = m_grid->GetCenterOfGrid();
    return QGeoCoordinate(crd.GetLatitude(),crd.GetLongitude() );
}


QObject* QTBackEnd::getCity(unsigned int id) {

    for(auto& it: m_cities){
        if(id ==(unsigned int) dynamic_cast<QTCity *>(it)->GetId()){
            return it;
        }
    }
    cout << "Could not find city with id: " << id << endl;
    return nullptr;

}

void QTBackEnd::runSimulator(unsigned int days) {

    if(!m_pop_generated){
        genPop();
    }

    if(m_grid == nullptr){
        cout << "run popgen first" << endl;
        return;
    }

    stride::SimRunner w(m_pt, m_grid->GetPopulation(), m_grid);
    w.Run();
    m_pop_generated = false;

    for(auto& it: m_cities){
        emit dynamic_cast<QTCity *>(it)->InfectedChanged();
    }
    emit SelectedInfectedChanged();
    emit TotalInfectedChanged();
}

//Config setters and getters
/***********************************************************************************************************************/

QString QTBackEnd::getConfig(const QString &xml_tag) const {
    return QString((m_geo_pt.get<string>(xml_tag.toStdString())).c_str());
}

void QTBackEnd::setConfig(const QString &xml_tag, const QString &val) {
    m_geo_pt.put(xml_tag.toStdString(),val.toStdString() );

}

QString QTBackEnd::readPath(const QString &tag) const{

    string complete_tag = "data_files." + tag.toStdString();
    auto file = m_geo_pt.get<string>(complete_tag);
    return QString(file.c_str());

}

QString QTBackEnd::setPath(const QString &tag, const QString &path) {

    string complete_tag = "data_files." + tag.toStdString();
    auto lst = path.split("/");
    m_geo_pt.put(complete_tag, lst.back().toStdString());
    return lst.back();
}

bool QTBackEnd::getBoolConfig(const QString &xml_tag) const{

    return m_geo_pt.get<bool>(xml_tag.toStdString());
}
bool QTBackEnd::setBoolConfig(const QString &xml_tag, const bool &value){

    m_geo_pt.put(xml_tag.toStdString(), value);

    return !value;
}

/***********************************************************************************************************************/


int QTBackEnd::CountSelectedInfected() {

    int counter = 0;
    for(auto& it : m_cities){
        auto * cty = dynamic_cast<QTCity*>(it);
        if(cty->IsClicked()){
            counter += cty->GetInfected();
        }
    }
    return counter;
}

int QTBackEnd::GetTotalInfected() {
    if(m_grid == nullptr){
        return 0;
    }
    int counter = 0;
    for(auto& it : m_grid->GetCities() ){
        counter += it.second.GetInfectedCount();
    }
    return counter;
}

void QTBackEnd::AddSelectedPop(int amount) {

    m_selected_pop += amount;
    emit SelectedPopChanged();
}

void QTBackEnd::AddCommuteLines(const QList<QTCommuter *> &lst) {

    for(auto& it: lst){
        m_commuters.append(it);
    }
    emit CommutersChanged();
}

void QTBackEnd::AddCommuteLineNoEmit(const QList<QTCommuter *> &lst) {
    for(auto& it: lst){
        m_commuters.append(it);
    }
}

void QTBackEnd::RemoveCommuteLines(const QList<QTCommuter *> &lst) {

    for(auto& it: lst){
        m_commuters.removeOne(it);
    }
    emit CommutersChanged();
}
void QTBackEnd::RemoveCommuteLinesNoEmit(const QList<QTCommuter *> &lst) {

    for(auto& it: lst){
        m_commuters.removeOne(it);
    }
}



void QTBackEnd::flipItems(QList<QObject *> cities) {
    for(auto& it: cities){
        auto cty = dynamic_cast<QTCity*>(it);
        if(cty != nullptr) {
            if (cty->IsClicked()) {
                RemoveCommuteLinesNoEmit(cty->GetCommuters());
            } else {
                AddCommuteLineNoEmit(cty->GetCommuters());
            }
            cty->Flip();
        }
        else{
            cout << "Some Items are in Flip items not cities" << endl;
        }

    }
    emit CommutersChanged();
    emit SelectedPopChanged();
    emit SelectedInfectedChanged();
}




