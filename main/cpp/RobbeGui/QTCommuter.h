//
// Created by robbe on 13.06.18.
//

#pragma once

#include <QObject>
#include <QtGui/QColor>
#include <QtPositioning/QGeoCoordinate>

#include "QTCity.h"

class QTCity;

class QTCommuter : public QObject {
    Q_OBJECT

public:

    Q_PROPERTY(QGeoCoordinate center_city1 MEMBER m_center_city1 CONSTANT)
    Q_PROPERTY(QGeoCoordinate center_city2 MEMBER m_center_city2 CONSTANT)
    Q_PROPERTY(double width READ calculate_line_width CONSTANT)
    Q_PROPERTY(QColor color READ pick_color CONSTANT)



    explicit QTCommuter(QTCity* city1 = nullptr, QTCity* city2 = nullptr, double outcommuting = 0, double incommuting = 0, QObject* parent = nullptr);
    ~QTCommuter() override = default;

    void set_in_commuters(double amount);
    QTCity* get_main_city();
    QTCity* get_second_city();

private:

    double calculate_line_width();
    QColor pick_color();
private:

QTCity* m_city1;
QTCity* m_city2;

QGeoCoordinate m_center_city1;
QGeoCoordinate m_center_city2;

double m_out_count;
double m_in_count;

};




