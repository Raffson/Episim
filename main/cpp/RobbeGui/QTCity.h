//
// Created by robbe on 04.06.18.
//

#pragma once

#include <QObject>
#include <QPoint>
#include <QGeoCoordinate>

#include "popgen-Episim/model/City.h"

class QTCity : public QObject {
    Q_OBJECT

public:
    explicit  QTCity(stride::City* m_city, QObject* = nullptr);
    explicit  QTCity(QObject* = nullptr);

    Q_INVOKABLE void ctyTest() {cout << "Hello from" << m_city->GetName() << endl;}
    Q_PROPERTY(QGeoCoordinate crd READ get_coordinates CONSTANT)
    Q_PROPERTY(int popCount READ get_population CONSTANT)


private:
    QGeoCoordinate get_coordinates() const;
    int get_population() const;
    

private:

   stride::City* m_city;


};
