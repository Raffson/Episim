//
// Created by robbe on 04.06.18.
//

#include "QTCity.h"

QTCity::QTCity(stride::City* model, QObject *parent ) : QObject(parent), m_city(model){

}

QTCity::QTCity(QObject *parent ) : QObject(parent){

}

int QTCity::get_population() const {
    return m_city->GetEffectivePopulation();
}

QGeoCoordinate QTCity::get_coordinates() const {
    return QGeoCoordinate(m_city->GetCoordinates().GetLatitude(), m_city->GetCoordinates().GetLongitude());
}
