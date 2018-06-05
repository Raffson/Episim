//
// Created by robbe on 04.06.18.
//

#pragma once

#include <QObject>
#include <QPoint>
#include <QGeoCoordinate>
#include <QString>
#include <QList>

#include "popgen-Episim/model/City.h"

#include <iostream>
using namespace std;

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
    Q_PROPERTY(QList<int> out_commuters READ get_out_commuters CONSTANT)
    Q_PROPERTY(QList<int> out_commuters_count READ get_out_commuters_count CONSTANT)
    Q_PROPERTY(QList<unsigned int> in_commuters READ get_in_commuters CONSTANT)
    Q_PROPERTY(QList<int> in_commuters_count READ get_in_commuters_count CONSTANT)

    stride::City* get_m_city(){return m_city;}

private:
    QGeoCoordinate get_coordinates() const;
    int get_population() const;
    QString get_name() const;
    int get_id() const;
    QList<int> get_out_commuters(){ return m_sorted_out_commuters;}
    QList<int> get_out_commuters_count(){ return m_commuter_out_count;}
    QList<unsigned int> get_in_commuters(){ return m_sorted_in_commuters;}
    QList<int> get_in_commuters_count(){ return m_commuter_in_count;}

    void fill_in_out_commuters();
    void fill_in_in_commuters();


    

private:

   stride::City* m_city;

   QList<int> m_sorted_out_commuters; ///> Keeps a list sorted high low to the commuters
   QList<int> m_commuter_out_count; ///> count ot amount of commuters

    QList<unsigned int> m_sorted_in_commuters; ///> Keeps a list sorted high low to the commuters
    QList<int> m_commuter_in_count; ///> count ot amount of commuters
};
