//
// Created by robbe on 04.06.18.
//

#pragma once

#include <QObject>
#include <QPoint>
#include <QGeoCoordinate>
#include <QString>

#include "popgen-Episim/model/City.h"

class QTCity : public QObject {
    Q_OBJECT

public:
    explicit  QTCity(stride::City* m_city, QObject* = nullptr);
    explicit  QTCity(QObject* = nullptr);

    Q_INVOKABLE void ctyTest() {cout << "Hello from" << m_city->GetName() << endl;}
    Q_PROPERTY(QGeoCoordinate crd READ get_coordinates CONSTANT)
    Q_PROPERTY(int popCount READ get_population CONSTANT)
    Q_PROPERTY(QString name READ get_name CONSTANT)
    Q_PROPERTY(int id READ get_id CONSTANT)

private:
    QGeoCoordinate get_coordinates() const;
    int get_population() const;
    QString get_name() const;
    int get_id() const;
    

private:

   stride::City* m_city;


};
