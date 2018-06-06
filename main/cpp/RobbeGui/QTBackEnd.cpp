//
// Created by robbe on 03.06.18.
//

#include "QTBackEnd.h"

QTBackEnd::QTBackEnd(ptree& pt, QObject *parent):QObject(parent),m_pt(pt) {

}

QTBackEnd::~QTBackEnd() {

    for(auto& it: m_cities){

        delete it;
    }
}



void QTBackEnd::genPop() {

    this->m_grid = stride::GeoGridGenerator().Generate(m_pt);
    stride::PopulationGenerator(*m_grid).Generate();
    makeCityList();
}

void QTBackEnd::makeCityList() {

    for(auto& it: m_cities){

        delete it;
    }
    m_cities.clear();

    for(auto& it: m_grid->GetCities()){
        m_cities.append(new QTCity(&it.second));
    }

}

QGeoCoordinate QTBackEnd::get_center() {

    stride::Coordinate crd = m_grid->GetCenterOfGrid();
    return QGeoCoordinate(crd.GetLatitude(),crd.GetLongitude() );
}

int QTBackEnd::get_total_pop() const {

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



