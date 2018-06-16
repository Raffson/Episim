/*
* Created by robbe on 03.06.18.
* Contains the QTbackend Class. Wraps the stride logic to be compatible with QT.
* This Way, the Stride logic has no QML dependencies.
*/

#pragma once

#include <memory>

#include <QObject>
#include <QList>
#include <QString>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtPositioning/QGeoCoordinate>

#include "boost/property_tree/ptree.hpp"

#include "popgen-Episim/generator/GeoGridGenerator.h"
#include "popgen-Episim/generator/PopulationGenerator.h"

#include "pop/Population.h"
#include "sim/Sim.h"

#include "QTCity.h"

using namespace boost::property_tree;
using namespace std;

namespace stride {
namespace gui {

class QTCity;

class QTCommuter;

/*
* Wraps the stride logic in a QML object.
* QTBackEnd is a singleton in our guid and provides
* logic handlers in QML.
*/
class QTBackEnd : public QObject {
Q_OBJECT

public:
// Constructors, Destructor
/************************************************************************************************************************/
/** @name Constructors, Destructor
*  List of constructors and Destructors.
*/
///@{
    /// @brief Constructor of BackEnd.
    /// @param engine the Qtquick engine.
    /// @param pt the config pt.
    explicit QTBackEnd(QQmlApplicationEngine &engine, ptree &pt, QObject *parent = nullptr);

    /// @brief default destructor
    ~QTBackEnd() override = default;

///@}
// Logic Invokables
/***********************************************************************************************************************/
/**
*  @name logic handlers.
*  Handlers for QML to initiate the logic in stride.
*/
    ///@{
    /// @brief Handler for QML to generate pop. Will do the genpop logic.
    Q_INVOKABLE void genPop();

    /// @brief Runs the simulator in n steps.
    /// @param days the amount of steps (days) to be run.
    Q_INVOKABLE void runSimulator(unsigned int days = 0);

    /// @brief tells QML if it should redraw the map items.
    /// This is needed if the population generator is rerun.
    /// @return inverse of pop_generated. If a pop is generated there should not be a redraw.
    Q_INVOKABLE bool shouldRedraw() const { return !m_pop_generated; };

    /// @brief This flips the state of a list of cities. Flipping from clicked to unclicked and vice versa.
    ///        mainly used if we select multiple cities with rectangle select.
    /// @param cities the list of cities to be flipped
    Q_INVOKABLE void flipItems(QList<QObject *> cities);

///@}
//Config Invokables
/***********************************************************************************************************************/
/**
* @name: Config Invokables
* All invokables to get_set the configuration.
*/
///@{
    /// @brief this reads a element at xml_tag.
    /// @param xml_tag tag where we can find the element needed.
    /// @return Returns the needed element as Qstring
    Q_INVOKABLE QString getConfig(const QString &xml_tag) const;

    /// @brief this sets a value in config at tag
    /// @param xml_tag the tag where we want to set our value.
    /// @param val the value that needs to be set in the config at tag as QString
    Q_INVOKABLE void setConfig(const QString &xml_tag, const QString &val);

    /// @brief reads a path out of config
    /// @param the tag where we find the path
    /// @return the path as a QString
    Q_INVOKABLE QString readPath(const QString &tag) const;

    /// @brief sets a path at tag
    /// @param tag tag where we need to place path in config
    /// @param path path to be placed in config
    Q_INVOKABLE QString setPath(const QString &tag, const QString &path);

    /// @brief gets a bool out of config at xml_tag
    /// @param xml_tag tag of the bool we need in config
    /// @return bool found in config
    Q_INVOKABLE bool getBoolConfig(const QString &xml_tag) const;

    /// @brief sets a bool in config file at xml_tag
    /// @param xml_tag the tag of our bool
    /// @param value the value as a bool we want to place
    Q_INVOKABLE bool setBoolConfig(const QString &xml_tag, const bool &value);

///@}
// BackEnd Properties. Wraps getters and setters. if those are trivial read/write the member directly
/***********************************************************************************************************************/
/**
* @name properties
* properties of backend in QML
*/
    ///@{
    ///< list of QCities made available in QMl
    Q_PROPERTY(QList<QObject *> cities
                       MEMBER
                               m_cities
                       NOTIFY
                       CitiesChanged)

    ///< the center of our grid. Needed for zooming
    Q_PROPERTY(QGeoCoordinate center
                       READ
                               GetCenter
                       CONSTANT)

    ///< the total population in our simulator
    Q_PROPERTY(int total_pop
                       MEMBER
                               m_total_pop
                       NOTIFY
                       PopChanged);

    ///< the total pop of our selected cities
    Q_PROPERTY(int selected_pop
                       MEMBER
                               m_selected_pop
                       NOTIFY
                       SelectedPopChanged)

    ///< the total infected of our selected cities
    Q_PROPERTY(int selected_infected
                       READ
                               CountSelectedInfected
                       NOTIFY
                       SelectedInfectedChanged())

    ///< The total amount of infected in our simulator
    Q_PROPERTY(int total_infected
                       READ
                               GetTotalInfected
                       NOTIFY
                       TotalInfectedChanged)

    ///< List of all commute lines. A bit non trivial but it was not possible to make a dynamic amount of views
    ///< a commute line object had the coordinates of 2 cities. and the in/out commuter on the line.
    Q_PROPERTY(QList<QObject *> commuters
                       MEMBER
                               m_commuters
                       NOTIFY
                       CommutersChanged)

///@}

// Invokable Getters
/***********************************************************************************************************************/
/**
* @name Invokable getters
* invokable getters in QML backend.
*/
///@{
    /// @brief Get's a QCity made in BackEnd.
    /// @param id Id of the needed city
    Q_INVOKABLE QObject *getCity(unsigned int id) const; // Is used in QML. Clion just does not recognize this
///@}
//Signals, are used to notify our views of change.
/***********************************************************************************************************************/
signals:
/**
* @name: signals
* Signals used to update the data in QML.
*/
    ///@{
    /// @brief tells QML that the selected pop needs to be updated.
    void SelectedPopChanged();

    /// @brief tells QML that the total pop needs to be updated.
    void PopChanged();

    /// @brief tells QML that the selected infected needs to be updated.
    void SelectedInfectedChanged();

    /// @brief tells QML that the selected infecteds needs to be updated.
    void TotalInfectedChanged();

    /// @brief tells QML that the city list is changed and needs to be updated.
    void CitiesChanged();

    /// @brief tells QML that the commuter lis is changed and needs to be updated.
    void CommutersChanged();
///@}
public:

/// Modifiers
/***********************************************************************************************************************/
/**
* @name: Modifiers
* Methods that modify BackEnd
*/
    ///@{
    /// @brief get's called by QTCities if they are selected. The pop_total get's updated.
    /// @param amount the pop that needs to be in/decremented to the selected pop total.
    void AddSelectedPop(int amount);

    void AddCommuteLines(const QList<QTCommuter *> &lst);

    void RemoveCommuteLines(const QList<QTCommuter *> &lst);
///@}

/// Data Retrieval
/***********************************************************************************************************************/
/**
* @name: Data Retrieval
* Collection of getters for data retrieval
*/
///@{
    /// @brief returns all QTCities.
    /// @return list of QTCities in the simulator.
    QList<QObject *> GetCities() const { return m_cities; }

    /// @brief returns the center of the GeoGrid based on the longitude and latitude of the cities.
    /// @return A QGeoCoordiante that contains the latitude en longitude of the center of the grid.
    QGeoCoordinate GetCenter() const;

    /// @brief returns the total infected in the grid.
    /// @return a integer value of the amount of infected.
    int GetTotalInfected() const;

    /// @brief Counts the selected infected
    /// @return a integer value of the amount of selected infected.
    int CountSelectedInfected() const;
///@}
private:

// Geogrid builder helpers
/***********************************************************************************************************************/
/**
* @name: Helpers to build the GeoGrid
*/
    ///@{
    /// Converts the stride::Cities by wrapping a QTCity around it.
    void MakeCityList();
///@}

// optimization functions
/***********************************************************************************************************************/
/**
* @name: Optimizers
* Functions to optimize rectangle select by reducing the amount of signals emitted
*/
    ///@{
    /// @brief Adds a list of commuter lines to the backend commuter lines. Doesn't emit any changes.
    /// @param lst a list of QTCommuter, lines that should be drawn.
    void AddCommuteLineNoEmit(const QList<QTCommuter *> &lst);

    /// @brief removes a list of commuter lines in the backend commuter lines. Doesn't emit any changes.
    /// @param lst list of Commuter lines to be removed.
    void RemoveCommuteLinesNoEmit(const QList<QTCommuter *> &lst);
///@}

private:
// Data members
//**********************************************************************************************************************/
    /**
     * @name: Data members
     * The data members
     */
///@{
    ///> Represents our grid
    shared_ptr<stride::GeoGrid> m_grid;

    ///> A Qlist that contains our QTCity models
    QList<QObject *> m_cities;

    ///> List of commuting lines that need to be drawn on the map
    QList<QObject *> m_commuters; // Commuting lines to be drawn?

    ///> property tree of stride config
    ptree &m_pt;

    ///> property tree of geo config
    ptree m_geo_pt;

    ///> application engine that runs our engine
    QQmlApplicationEngine &m_engine;

    ///> keeps track if there is a gnerated pop that can be used to run the simulator on
    bool m_pop_generated{false};

    ///> Total pop in our grid
    int m_total_pop{0};

    ///> Pop that is selected
    int m_selected_pop{0};
///@}



};

}
}