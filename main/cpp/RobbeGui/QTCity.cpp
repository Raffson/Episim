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

QString QTCity::get_name() const {
    return QString(m_city->GetName().c_str());
}

int QTCity::get_id() const {
    return m_city->GetId();
}
