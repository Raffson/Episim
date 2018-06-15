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
    emit popChanged();

    makeCityList();
}

void QTBackEnd::makeCityList() {


    m_cities.clear();
    for(auto& it: m_grid->GetCities()){
        m_cities.append(new QTCity(&it.second, this));
    }
    for(auto& it: m_cities){ // Needs to happen when the full list is initialized
        auto cty = dynamic_cast<QTCity*>(it);

        cty->create_commuting_lst(10);
    }
    emit citiesChanged();
}

QGeoCoordinate QTBackEnd::get_center() {

    stride::Coordinate crd = m_grid->GetCenterOfGrid();
    return QGeoCoordinate(crd.GetLatitude(),crd.GetLongitude() );
}


QObject* QTBackEnd::get_city(unsigned int id) {

    for(auto& it: m_cities){
        if(id ==(unsigned int) dynamic_cast<QTCity*>(it)->get_id()){
            return it;
        }
    }
    cout << "Could not find city with id: " << id << endl;
    return nullptr;

}

void QTBackEnd::run_simulator(unsigned int days) {

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
        emit dynamic_cast<QTCity*>(it)->infectedChanged();
    }
    emit selected_infectedChanged();
    emit total_infectedChanged();
}

QString QTBackEnd::get_config(QString xml_tag) {
    return QString((m_geo_pt.get<string>(xml_tag.toStdString())).c_str());
}

void QTBackEnd::set_config(QString xml_tag, QString val) {
    cout << val.toStdString() << endl;
    m_geo_pt.put(xml_tag.toStdString(),val.toStdString() );

}

int QTBackEnd::count_selected_infected() {

    int counter = 0;
    for(auto& it : m_cities){
        auto * cty = dynamic_cast<QTCity*>(it);
        if(cty->get_clicked()){
            counter += cty->get_infected();
        }
    }
    return counter;
}

int QTBackEnd::get_total_infected() {
    if(m_grid == nullptr){
        return 0;
    }
    int counter = 0;
    for(auto& it : m_grid->GetCities() ){
        counter += it.second.GetInfectedCount();
    }
    return counter;
}

void QTBackEnd::add_selected_pop(int amount) {

    m_selected_pop += amount;
    emit selected_popChanged();
}

void QTBackEnd::add_commute_lines(const QList<QTCommuter*> &lst) {

    for(auto& it: lst){
        m_commuters.append(it);
    }
    emit commutersChanged();
}

void QTBackEnd::add_commute_lines_no_emit(const QList<QTCommuter*> &lst) {
    for(auto& it: lst){
        m_commuters.append(it);
    }
}

void QTBackEnd::remove_commute_lines(const QList<QTCommuter*> &lst) {

    for(auto& it: lst){
        m_commuters.removeOne(it);
    }
    emit commutersChanged();
}
void QTBackEnd::remove_commute_lines_no_emit(const QList<QTCommuter*> &lst) {

    for(auto& it: lst){
        m_commuters.removeOne(it);
    }
}



void QTBackEnd::flip_items(QList<QObject*> cities) {

    for(auto& it: cities){
        auto cty = dynamic_cast<QTCity*>(it);
        if(cty->get_clicked()){
            remove_commute_lines_no_emit(cty->get_commuters());
        }
        else{
            add_commute_lines_no_emit(cty->get_commuters());
        }
        cty->flip();

    }
    emit commutersChanged();
    emit selected_popChanged();
    emit selected_infectedChanged();
}



