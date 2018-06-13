//
// Created by robbe on 13.06.18.
//

#pragma once

#include <QObject>
#include "QtPositioning/QGeoCoordinate"

#include "popgen-Episim/model/Coordinate.h"


class QTCommuter : public QObject {

public:
    QTCommuter(stride::Coordinate c1, stride::Coordinate c2, QObject* parent = nullptr);
    ~QTCommuter() = default;


private:

QGeoCoordinate m_c1;
QGeoCoordinate m_c2;


};



