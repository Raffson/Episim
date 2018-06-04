//
// Created by robbe on 04.06.18.
//

#include "QTCity.h"

QTCity::QTCity(stride::City* model, QObject *parent ) : QObject(parent), m_city(model){

}

QGeoCoordinate QTCity::get_coordinates() {
    return QGeoCoordinate(m_city->GetCoordinates().GetLongitude(), m_city->GetCoordinates().GetLatitude());
}
