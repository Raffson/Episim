//
// Created by robbe on 03.06.18.
//

#pragma once

#include <memory>

#include <QObject>
#include <QList>
#include <QString>
#include <QGeoCoordinate>
#include <QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>

#include "boost/property_tree/ptree.hpp"

#include "popgen-Episim/generator/GeoGridGenerator.h"
#include "popgen-Episim/generator/PopulationGenerator.h"

#include "pop/Population.h"
#include "sim/Sim.h"

#include "QTCity.h"

using namespace boost::property_tree;
using namespace std;


class QTCity;

class QTBackEnd: public QObject {
    Q_OBJECT

public:

    explicit QTBackEnd(QQmlApplicationEngine& engine, ptree& pt, QObject *parent = nullptr);

    /// @brief Handler for QML to generate pop. Will do the genpop logic.
    Q_INVOKABLE void genPop();
    Q_INVOKABLE QObject* get_city(unsigned int id);
    Q_INVOKABLE void run_simulator(unsigned int days = 0);
    Q_INVOKABLE bool should_redraw(){return !m_pop_generated;};


    Q_INVOKABLE QString get_config(QString xml_tag);
    Q_INVOKABLE void set_config(QString xml_tag, QString val);


    Q_PROPERTY(QList<QObject*> cities READ get_cities CONSTANT)
    Q_PROPERTY(QGeoCoordinate center READ get_center CONSTANT)
    Q_PROPERTY(int total_pop READ get_total_pop CONSTANT);
    Q_PROPERTY(int selected_infected READ count_selected_infected NOTIFY selected_infectedChanged())

    signals:
    void selected_infectedChanged();




private:
    void makeCityList();
    QList<QObject*> get_cities(){return m_cities;}
    QGeoCoordinate get_center();
    int get_total_pop() const;
    int count_selected_infected();




private:

    ///> Represents our grid
    shared_ptr<stride::GeoGrid> m_grid;

    ///> A Qlist that conains our QTCity models
    QList<QObject*> m_cities;

    ptree& m_pt;
    ptree  m_geo_pt;
    QQmlApplicationEngine& m_engine;

    bool m_pop_generated{false};

};

