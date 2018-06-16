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
    Q_INVOKABLE QObject* getCity(unsigned int id);
    Q_INVOKABLE void runSimulator(unsigned int days = 0);
    Q_INVOKABLE bool shouldRedraw(){return !m_pop_generated;};
    Q_INVOKABLE void flipItems(QList<QObject *> cities);

//Config Invokables
/************************************************************************************************************************/
    Q_INVOKABLE QString getConfig(QString xml_tag);
    Q_INVOKABLE void setConfig(QString xml_tag, QString val);
    Q_INVOKABLE QString readPath(QString tag);
    Q_INVOKABLE QString setPath(QString tag, QString path);
    Q_INVOKABLE bool getBoolConfig(QString xml_tag);
    Q_INVOKABLE bool setBoolConfig(QString xml_tag, bool value);


    Q_PROPERTY(QList<QObject*> cities MEMBER m_cities NOTIFY CitiesChanged)
    Q_PROPERTY(QGeoCoordinate center READ GetCenter CONSTANT)

    Q_PROPERTY(int total_pop MEMBER m_total_pop NOTIFY PopChanged);
    Q_PROPERTY(int selected_pop MEMBER m_selected_pop NOTIFY SelectedPopChanged)
    Q_PROPERTY(int selected_infected READ CountSelectedInfected NOTIFY
                       SelectedInfectedChanged())
    Q_PROPERTY(int total_infected READ GetTotalInfected NOTIFY TotalInfectedChanged)
    Q_PROPERTY(QList<QObject*> commuters MEMBER m_commuters NOTIFY CommutersChanged)

signals:
    void SelectedPopChanged();
    void PopChanged();
    void SelectedInfectedChanged();
    void TotalInfectedChanged();
    void CitiesChanged();
    void CommutersChanged();

public:
    void AddSelectedPop(int amount);
    QList<QObject*> GetCities(){return m_cities;}

    void AddCommuteLines(const QList<QTCommuter *> &lst);
    void RemoveCommuteLines(const QList<QTCommuter *> &lst);

private:
    void MakeCityList();

    QGeoCoordinate GetCenter();
    int CountSelectedInfected();
    int GetTotalInfected();

    void AddCommuteLineNoEmit(const QList<QTCommuter *> &lst);
    void RemoveCommuteLinesNoEmit(const QList<QTCommuter *> &lst);




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

