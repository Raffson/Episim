//
// Created by robbe on 03.06.18.
//

#pragma once

#include <QObject>
#include <QList>
#include <QGeoCoordinate>
#include <QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>

#include "boost/property_tree/ptree.hpp"

#include "popgen-Episim/generator/GeoGridGenerator.h"
#include "popgen-Episim/generator/PopulationGenerator.h"

#include "pop/Population.h"
#include "sim/SimRunner.h"
using namespace boost::property_tree;

#include "QTCity.h"



class QTBackEnd: public QObject {
    Q_OBJECT

public:

    explicit QTBackEnd(QQmlApplicationEngine& engine, ptree& pt, QObject *parent = nullptr);

    /// @brief Handler for QML to generate pop. Will do the genpop logic.
    Q_INVOKABLE void genPop();
    Q_INVOKABLE QObject* get_city(unsigned int id);
    Q_INVOKABLE void run_simulator(unsigned int days = 0);


    Q_PROPERTY(QList<QObject*> cities READ get_cities)
    Q_PROPERTY(QGeoCoordinate center READ get_center)
    Q_PROPERTY(int total_pop READ get_total_pop CONSTANT);



private:
    void makeCityList();
    QList<QObject*> get_cities(){return m_cities;}
    QGeoCoordinate get_center();
    int get_total_pop() const;




private:

    ///> Represents our grid
    shared_ptr<stride::GeoGrid> m_grid;

    ///> A Qlist that conains our QTCity models
    QList<QObject*> m_cities;

    ptree& m_pt;
    QQmlApplicationEngine& m_engine;



};

