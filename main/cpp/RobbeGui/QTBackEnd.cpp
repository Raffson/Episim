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


QTBackEnd::QTBackEnd(QQmlApplicationEngine& engine, ptree& pt, QObject *parent):QObject(parent),
                    m_pt(pt),m_engine(engine) {
    m_engine.rootContext()->setContextProperty("CityModel", QVariant::fromValue(m_cities));

    string file(m_pt.get<string>("run.geopop_file"));
    string path = "config/" + file;
    read_xml(path, m_geo_pt);
}


void QTBackEnd::genPop() {

    this->m_grid = stride::GeoGridGenerator().Generate(m_pt);
    stride::PopulationGenerator(*m_grid).Generate();
    m_pop_generated = true;
    //emit popChanged();
    makeCityList();
}

void QTBackEnd::makeCityList() {


    m_cities.clear();

    for(auto& it: m_grid->GetCities()){
        m_cities.append(new QTCity(&it.second, this));
    }



    m_engine.rootContext()->setContextProperty("CityModel", QVariant::fromValue(m_cities));
}

QGeoCoordinate QTBackEnd::get_center() {

    stride::Coordinate crd = m_grid->GetCenterOfGrid();
    return QGeoCoordinate(crd.GetLatitude(),crd.GetLongitude() );
}

int QTBackEnd::get_total_pop() const {

    if(m_grid == nullptr){
        return 0;
    }
    return m_grid->GetTotalPop();
}

QObject *QTBackEnd::get_city(unsigned int id) {

    for(auto& it: m_cities){
        auto tmp = dynamic_cast<QTCity*>(it);
        if(id == tmp->get_m_city()->GetId()){
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

    m_geo_pt.put(val.toStdString(), xml_tag.toStdString());

}

int QTBackEnd::count_selected_infected() {

    int counter = 0;
    for(auto& it : m_cities){
        QTCity* cty = dynamic_cast<QTCity*>(it);
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



