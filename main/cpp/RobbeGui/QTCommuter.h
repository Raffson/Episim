//
// Created by robbe on 13.06.18.
//

#pragma once

#include <QObject>
#include "QtPositioning/QGeoCoordinate"
#include "popgen-Episim/model/Coordinate.h"
#include "QTCity.h"

class QTCommuter : public QObject {

public:
    QTCommuter(QTCity* city1, QTCity* city2, QObject* parent = nullptr);
    ~QTCommuter() = default;


private:

QTCity* m_city1;
QTCity* m_city2;



};



