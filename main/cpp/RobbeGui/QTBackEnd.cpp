//
// Created by robbe on 03.06.18.
//

#include "QTBackEnd.h"

QTBackEnd::QTBackEnd(QObject *parent): QObject(parent) {

}

QTBackEnd::~QTBackEnd() {

    for(auto& it: m_cities){

        delete it;
    }
}



void QTBackEnd::genPop() {

    this->m_grid = stride::GeoGridGenerator().Generate("run_default.xml");
    stride::PopulationGenerator(*m_grid).Generate();
    makeCityList();
}

void QTBackEnd::makeCityList() {

    for(auto& it: m_grid->GetCities()){
        m_cities.append(new QTCity(&it.second));
    }

}

QGeoCoordinate QTBackEnd::get_center() {

    stride::Coordinate crd = m_grid->GetCenterOfGrid();
    return QGeoCoordinate(crd.GetLatitude(),crd.GetLongitude() );
}

