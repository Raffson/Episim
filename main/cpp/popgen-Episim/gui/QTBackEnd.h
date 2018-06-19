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


#include "sim/CliController.h"
#include "boost/property_tree/ptree.hpp"
#include "popgen-Episim/generator/GeoGridGenerator.h"
#include "popgen-Episim/generator/PopulationGenerator.h"
#include "pop/Population.h"
#include "QTCity.h"

using namespace boost::property_tree;
using namespace std;

namespace stride {
class CliController;
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
    /// @param engine: The Qtquick engine.
    /// @param pt: The config pt.
    /// @param clicontrol: The pointer to the viewer.
    /// @param parent: Optional Used in QML to be a MetaObject of QT.
    explicit QTBackEnd(QQmlApplicationEngine &engine, ptree &pt, CliController *clicontrol,
                       QObject *parent = nullptr);

    /// QTBackend should be unique to a QMLApplication. no copying.
    QTBackEnd(const QTBackEnd&)=delete;
    QTBackEnd& operator=(const QTBackEnd&)=delete;

    /// @brief Default destructor.
    ~QTBackEnd() override = default;

///@}
// Logic Handlers
/***********************************************************************************************************************/
/**
*  @name Logic Handlers
*  Handlers for QML to initiate the logic in stride.
*/
    ///@{
    /// @brief Handler for QML to generate pop. Will do the genpop logic.
    Q_INVOKABLE void genPop();

    /// @brief Runs the simulator in n steps.
    /// @param days: The amount of steps (days) to be run.
    /// @param all: Indicates whether all days should be simulated at once.
    Q_INVOKABLE void runSimulator(int run_val, bool all);

    /// @brief Tells QML if it should redraw the map items.
    /// This is needed if the population generator is rerun.
    /// @return Inverse of pop_generated. If a pop is generated there should not be a redraw.
    Q_INVOKABLE bool shouldRedraw() const { return !m_pop_generated; };

    /// @brief This flips the state of a list of cities. Flipping from clicked to unclicked and vice versa.
    ///        Mainly used if we select multiple cities with rectangle select.
    /// @param cities: The list of cities to be flipped.
    Q_INVOKABLE void flipItems(QList<QObject *> cities);

///@}
//Config Invokables
/***********************************************************************************************************************/
/**
* @name: Config Invokables
* All invokables to get_set the configuration. If there was a way to use templates in QML this would be alot shorter.
*/
///@{
    /// @brief Reads an element at xml_tag.
    /// @param xml_tag: Tag where we can find the element needed.
    /// @param geo: Optional read out of geo or stride ptree.
    /// @return Returns the needed element as Qstring.
    Q_INVOKABLE QString getConfig(const QString &xml_tag, bool geo = true) const;

    /// @brief Sets a value in config at tag.
    /// @param xml_tag: The tag where we want to set our value.
    /// @param val: The value that needs to be set in the config at tag as QString.
    /// @param geo: Optional read out of geo or stride ptree.
    Q_INVOKABLE void setConfig(const QString &xml_tag, const QString &val, bool geo = true);

    /// @brief Reads a path out of config of a file that should be in the data folder if install path's are on.
    /// @param tag: Tag where we find the path.
    /// @param geo_grid: Config out of the GeoGrid file or GenPop file?
    /// @param data Data: file our config file?
    /// @return The path as a QString.
    Q_INVOKABLE QString readPath(const QString &tag, bool geo_grid = true, bool data = true) const;

    /// @brief Sets a path at tag.
    /// @param tag: Tag where we need to place path in config.
    /// @param path: Path to be placed in config.
    /// @param geo_grid: Config out of the GeoGrid file or GenPop file?
    Q_INVOKABLE void setPath(const QString &tag, const QString &path, bool geoGrid = true);

    /// @brief Gets a bool out of config at xml_tag.
    /// @param xml_tag: Tag of the bool we need in config.
    /// @param geo: Tag if we write in geo or stride config.
    /// @return Bool found in config.
    Q_INVOKABLE bool getBoolConfig(const QString &xml_tag, bool geo = true) const;

    /// @brief Sets a bool in config file at xml_tag.
    /// @param xml_tag: The tag of our bool.
    /// @param value the value as a bool we want to place
    /// @param geo in geo pr or stride pt
    Q_INVOKABLE bool setBoolConfig(const QString &xml_tag, const bool &value, bool geo);

    /// @brief Wrapper to get the path to data.
    /// @return Returns a QUrl path to the Data folder.
    Q_INVOKABLE QUrl getDataPath() const ;

    /// @brief Wrapper to get the pat do config.
    /// @return Returns a QUrl path to the config folder.
    Q_INVOKABLE QUrl getConfigPath() const ;

    /// @brief Counts the children of a certain p_tree element.
    /// @param xml_tag: Tag of p_tree we want to count our childen from.
    Q_INVOKABLE int countConfigChildren(const QString &xml_tag, bool geo) const;

    /// @brief Gets a child at index. Due to Keeping logic out of QML it will add an item with 0 value at the end
    ///        if the index > then the amount of children at the tag.
    /// @param xml_tag: Tag to read the children from.
    /// @param index: The index to get the child from.
    /// @param geo: Read out of geogrid config or stride config.
    Q_INVOKABLE double getChildAtIndex(const QString &xml_tag, int index, bool geo);

    /// @brief Sets the value at index of a child of the xml tag.
    /// @param xml_tag: The tag of the childeren we are working on.
    /// @param index: The index we want to update.
    /// @param geo: Using geo or stride config.
    Q_INVOKABLE void setChildAtIndex(const QString &xml_tag, int index, double value, bool geo);

    /// @brief cleans the children till the parent has as much children as size
    /// @param xml_tag of the parent of the children we want ro remove
    /// @param size how many children we want the parent to have
    /// @param geo is this tag in the geo or m pt
    Q_INVOKABLE void cleanChildren (const QString& xml_tag, int size, bool geo);

    /// @brief returns a day, month, year of a date at xml tag
    /// @param xml_tag where to find date.
    /// @param index for day = 0, month = 1, year = 2
    /// @param bool in geo or stride
    Q_INVOKABLE int getDate(const QString& xml_tag, int index, bool geo);

    /// @brief puts a date at xml_tag
    /// @param day the day of the date
    /// @param month the month of the date
    /// @param year the year of he date
    /// @param geo in geo or stride?
    Q_INVOKABLE void setDate(const QString& xml_tag, int day, int month, int year, bool geo);


///@}
// BackEnd Properties. Wraps getters and setters. if those are trivial read/write the member directly
/***********************************************************************************************************************/
/**
* @name BackEnd Properties
* Properties of backend in QML.
*/
    ///@{
    ///< List of QCities made available in QMl.
    Q_PROPERTY(QList<QObject *> cities MEMBER m_cities NOTIFY CitiesChanged)

    ///< The center of our grid. Needed for zooming.
    Q_PROPERTY(QGeoCoordinate center READ GetCenter CONSTANT)

    ///< The total population in our simulator.
    Q_PROPERTY(int total_pop MEMBER m_total_pop NOTIFY PopChanged)

    ///< The total population of our selected cities.
    Q_PROPERTY(int selected_pop MEMBER m_selected_pop NOTIFY SelectedPopChanged)

    ///< The total amount of infected of our selected cities.
    Q_PROPERTY(int selected_infected READ CountSelectedInfected NOTIFY SelectedInfectedChanged())

    ///< The total amount of infected in our simulator.
    Q_PROPERTY(int total_infected READ GetTotalInfected NOTIFY TotalInfectedChanged)

    ///< List of all commute lines. A bit non trivial but it was not possible to make a dynamic amount of views.
    ///< A commute line object has the coordinates of 2 cities and the in/out commuters on the line.
    Q_PROPERTY(QList<QObject *> commuters MEMBER m_commuters NOTIFY CommutersChanged)

///@}

// Invokable Getters
/***********************************************************************************************************************/
/**
* @name Invokable getters
* Invokable getters in QML backend.
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
* @name: Signals
* Signals used to update the data in QML.
*/
    ///@{
    /// @brief Tells QML that the selected pop needs to be updated.
    void SelectedPopChanged();

    /// @brief Tells QML that the total pop needs to be updated.
    void PopChanged();

    /// @brief Tells QML that the selected infected needs to be updated.
    void SelectedInfectedChanged();

    /// @brief Tells QML that the selected infected needs to be updated.
    void TotalInfectedChanged();

    /// @brief Tells QML that the city list is changed and needs to be updated.
    void CitiesChanged();

    /// @brief Tells QML that the commuter list is changed and needs to be updated.
    void CommutersChanged();
///@}
public:

/// Modifiers
/***********************************************************************************************************************/
/**
* @name: Modifiers
* Methods that modify BackEnd.
*/
    ///@{
    /// @brief Get's called by QTCities if they are selected. The pop_total get's updated.
    /// @param Amount the selected pop total needs to be in/decremented with
    void AddSelectedPop(int amount);

    void AddCommuteLines(const QList<QTCommuter *> &lst);

    void RemoveCommuteLines(const QList<QTCommuter *> &lst);
///@}

/// Data Retrieval
/***********************************************************************************************************************/
/**
* @name: Data Retrieval
* Collection of getters for data retrieval.
*/
///@{
    /// @brief Returns all QTCities.
    /// @return list of QTCities in the simulator.
    QList<QObject *> GetCities() const { return m_cities; }

    /// @brief Returns the center of the GeoGrid based on the longitude and latitude of the cities.
    /// @return A QGeoCoordiante that contains the latitude en longitude of the center of the grid.
    QGeoCoordinate GetCenter() const;

    /// @brief Returns the total infected in the grid.
    /// @return An integer value of the amount of infected.
    int GetTotalInfected() const;

    /// @brief Counts the selected infected.
    /// @return An integer value of the amount of selected infected.
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
* Functions to optimize rectangle select by reducing the amount of signals emitted.
*/
    ///@{
    /// @brief Adds a list of commuter lines to the backend commuter lines. Doesn't emit any changes.
    /// @param lst a list of QTCommuter, lines that should be drawn
    void AddCommuteLineNoEmit(const QList<QTCommuter *> &lst);

    /// @brief Removes a list of commuter lines in the backend commuter lines. Doesn't emit any changes.
    /// @param lst list of Commuter lines to be removed
    void RemoveCommuteLinesNoEmit(const QList<QTCommuter *> &lst);

///@}

private:
// Data members
//**********************************************************************************************************************/
    /**
     * @name: Data members
     * The data members.
     */
///@{
    ///> Represents our grid.
    shared_ptr<stride::GeoGrid> m_grid;

    ///> A Qlist that contains our QTCity models.
    QList<QObject *> m_cities;

    ///> List of commuting lines that need to be drawn on the map.
    QList<QObject *> m_commuters; // Commuting lines to be drawn?

    ///> Property tree of stride config.
    ptree &m_pt;

    ///> Property tree of geo config.
    ptree m_geo_pt;

    ///> Application engine that runs our engine.
    QQmlApplicationEngine &m_engine;

    ///> Keeps track if there is a generated population that can be used to run the simulator on.
    bool m_pop_generated{false};

    ///> Total population in our grid.
    int m_total_pop{0};

    ///> Population that is selected.
    int m_selected_pop{0};

    ///> Pointer to optional controller -> used to registerViewers.
   CliController* m_cntrller;

   ///> Pointer to the current runner
   shared_ptr<SimRunner> m_runner{nullptr};
///@}



};

}
}