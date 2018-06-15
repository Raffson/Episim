//
// Created by robbe on 03.06.18.
//

#pragma once

#include <memory>

#include <QObject>
#include <QList>
#include <QString>
#include <QtGui/QGuiApplication>
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
class QTCommuter;

class QTBackEnd: public QObject {
    Q_OBJECT

public:

    explicit QTBackEnd(QQmlApplicationEngine& engine, ptree& pt, QObject *parent = nullptr);
    ~QTBackEnd() = default;

// Logic Invokables
/***********************************************************************************************************************/
    /// @brief Handler for QML to generate pop. Will do the genpop logic.
    Q_INVOKABLE void genPop();
    Q_INVOKABLE QObject* get_city(unsigned int id);
    Q_INVOKABLE void run_simulator(unsigned int days = 0);
    Q_INVOKABLE bool should_redraw(){return !m_pop_generated;};
    Q_INVOKABLE void flip_items(QList<QObject*>);

//Config Invokables
/************************************************************************************************************************/
    Q_INVOKABLE QString get_config(QString xml_tag);
    Q_INVOKABLE void set_config(QString xml_tag, QString val);
    Q_INVOKABLE QString read_path(QString tag);
    Q_INVOKABLE QString set_path(QString tag, QString path);
    Q_INVOKABLE bool get_bool_config(QString xml_tag);
    Q_INVOKABLE bool set_bool_config(QString xml_tag, bool value);


    Q_PROPERTY(QList<QObject*> cities MEMBER m_cities NOTIFY citiesChanged)
    Q_PROPERTY(QGeoCoordinate center READ get_center CONSTANT)

    Q_PROPERTY(int total_pop MEMBER m_total_pop NOTIFY popChanged);
    Q_PROPERTY(int selected_pop MEMBER m_selected_pop NOTIFY selected_popChanged)
    Q_PROPERTY(int selected_infected READ count_selected_infected NOTIFY selected_infectedChanged())
    Q_PROPERTY(int total_infected READ get_total_infected NOTIFY total_infectedChanged)
    Q_PROPERTY(QList<QObject*> commuters MEMBER m_commuters NOTIFY commutersChanged)

signals:
    void selected_popChanged();
    void popChanged();
    void selected_infectedChanged();
    void total_infectedChanged();
    void citiesChanged();
    void commutersChanged();

public:
    void add_selected_pop(int amount);
    QList<QObject*> get_cities(){return m_cities;}

    void add_commute_lines(const QList<QTCommuter*>& lst);
    void remove_commute_lines(const QList<QTCommuter *> &lst);




private:
    void makeCityList();

    QGeoCoordinate get_center();
    int count_selected_infected();
    int get_total_infected();

    void add_commute_lines_no_emit(const QList<QTCommuter*>& lst);
    void remove_commute_lines_no_emit(const QList<QTCommuter *> &lst);




private:

    ///> Represents our grid
    shared_ptr<stride::GeoGrid> m_grid;

    ///> A Qlist that conains our QTCity models
    vector<QTCity> m_effective_cities;
    QList<QObject*> m_cities;
    QList<QObject*> m_commuters; // Commuting lines to be drawn?

    ptree& m_pt;
    ptree  m_geo_pt;
    QQmlApplicationEngine& m_engine;

    bool m_pop_generated{false};
    int m_total_pop{0};
    int m_selected_pop{0};



};

