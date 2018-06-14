//
// Created by robbe on 13.06.18.
//

#pragma once

#include <QObject>
#include "QtPositioning/QGeoCoordinate"
#include "popgen-Episim/model/Coordinate.h"
#include "QTCity.h"

class QTCity;

class QTCommuter : public QObject {
    Q_OBJECT

public:

    Q_PROPERTY(QGeoCoordinate center_city1 MEMBER m_center_city1 CONSTANT)
    Q_PROPERTY(QGeoCoordinate center_city2 MEMBER m_center_city2 CONSTANT)



    explicit QTCommuter(QTCity* city1 = nullptr, QTCity* city2 = nullptr, int outcommuting = 0, int incommuting = 0, QObject* parent = nullptr);
    ~QTCommuter() override = default;

    void set_in_commuters(int amount);
    QTCity* get_main_city();

private:
private:

QTCity* m_city1;
QTCity* m_city2;

QGeoCoordinate m_center_city1;
QGeoCoordinate m_center_city2;

int m_out_count;
int m_in_count;

int m_line_width{0};
};




