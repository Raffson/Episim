//
// Created by robbe on 04.06.18.
// Contains the QTCity wrapper to use Cities in QT QML.
//

#pragma once

#include "popgen-Episim/model/City.h"
#include "QTBackEnd.h"
#include "QTCommuter.h"

using namespace std;



/**
 * @brief: Wrapper For Stride City. This enables to use a citie object in QML Code.
 */
namespace stride {
namespace gui {
class QTBackEnd;
class QTCommuter;
class QTCity : public QObject {
Q_OBJECT

public:
//Constructors, Destructor
/***********************************************************************************************************************/
/**
* @name: Constructors, Destructors
*/
    ///@{
    /// @brief Constructor of QCity. Note: could be used as QTCity() default so it can be a MetaObject in QML.
    /// @param m_city: The city model that corresponds to this QTCity.
    /// @param back_end: Pointer to the back_end that is a parent of QTCity.
    /// @param parent: Qobject parent used in QML, we use it to call QObject constructor.
    explicit QTCity(stride::City *m_city = nullptr, QTBackEnd *back_end = nullptr, QObject *parent = nullptr);

    /// @brief Copy constructor. Makes a non trivial deep copy of QTCity.
    /// @param object: QTCity to be copied.
    QTCity(const QTCity &object);

    QTCity& operator=(const QTCity&)=delete;

    /// @brief Default destructor.
    ~QTCity() override = default;
///@}

/// Q Properties
/***********************************************************************************************************************/
/**
* @name: QProperties
* Properties available to the City Qml type.
*/
///@{
    ///< The coordinate of our city center.
    Q_PROPERTY(QGeoCoordinate crd
                       READ
                               GetCoordinates
                       CONSTANT)

    ///< Population amount in city.
    Q_PROPERTY(int popCount
                       MEMBER
                               m_pop
                       CONSTANT)

    ///< Name of the city.
    Q_PROPERTY(QString name
                       READ
                               GetName
                       CONSTANT)

    ///< Id of the city.
    Q_PROPERTY(int id
                       READ
                               GetId
                       CONSTANT)

    ///< Total commuters into the city.
    Q_PROPERTY(int total_in_commuters
                       READ
                               GetTotalCommutersIn
                       CONSTANT)

    ///< Total commuters out of the city.
    Q_PROPERTY(int total_out_commuters
                       READ
                               GetTotalCommutersOut
                       CONSTANT)

    ///< Infected in the city.
    Q_PROPERTY(int infected
                       READ
                               GetInfected
                       NOTIFY
                       InfectedChanged)

    ///< Clicked state of the city.
    Q_PROPERTY(bool clicked
                       READ
                               IsClicked
                       WRITE
                               SetClicked
                       NOTIFY
                       ClickedChanged)
///@}

// City signals
/***********************************************************************************************************************/
signals:
/**
* @name: City signals
* All signals of QTCity.
*/
    ///@{
    ///@brief Notifies QML that the infected changed.
    void InfectedChanged();

    ///@brief Notifies QML that the clicked state changed.
    void ClickedChanged();
///@}

public:
// Retrieval
/***********************************************************************************************************************/
/**
* @name Retrieval
* All the getters of City.
*/
    ///@{
    ///@brief Returns a poiner to the model city.
    ///@return A City* to the model city.
    stride::City *GetModelCity() const { return m_city; }

    ///@brief Returns a pointer to the back end.
    ///@return Returns a pointer to the back end cbject that is the parent of this city.
    QTBackEnd *GetBackEnd() const { return m_back_end; }

    ///@brief Returns the infected count in the city.
    ///@return An integer that represents the infected count of this city.
    int GetInfected() const { return (int) m_city->GetInfectedCount(); }

    ///@brief Returns the clicked state of our city.
    ///@return A bool representing the clicked state of the city.
    bool IsClicked() const { return m_is_clicked; }

    ///@brief Returns the id of the city.
    ///@return An integer representing the id of the city.
    int GetId() const;

    ///@brief Returns the coordinates of the city.
    ///@returns A QGeocoordinate representing the coordinate of the city in latitude and longitude.
    QGeoCoordinate GetCoordinates() const;

    ///@brief Returns the commuters of the city.
    ///@return A list of QTCommuters representing our commuter lines.
    const QList<QTCommuter *> &GetCommuters() const { return m_commuting_lst; }

    ///@brief Returns the name of the city.
    ///@return A Qstring with the name of the city.
    QString GetName() const;

    ///@brief Returns an integer value of the outcommuters.
    ///@return An integer value of totalOutCommuters.
    int GetTotalCommutersOut() { return (int) m_city->GetTotalOutCommutersCount(); }

    ///@brief Returns an integer value of the incommuters.
    ///@return Returns an ingterer value of the incommuters
    int GetTotalCommutersIn() { return (int) m_city->GetTotalInCommutersCount(); }
///@}

// Modifiers
/************************************************************************************************************************/
/**
* @name Modifiers
* All the modifiers of city.
*/
///@{
    /// @brief Flips the state of our city between clicked/not cicked.
    void Flip();

    /// @brief Sets the state of our city and handles all emits nescesary.
    /// @param val: Bool representing the new clicked state.
    void SetClicked(bool val);

    ///@brief Creates the commuting list. Note that this has to happen after ALL QTCities are created otherwise
    ///       we try to refer not yet existing cities.
    ///@param amount: The amount of in and out commuters to be made.
    void CreateCommutingList(int amount);
///@}

// Data members
/***********************************************************************************************************************/
private:
/**
* @name Datamembers
* The data members.
*/
    ///@{
    ///< Pointer to the model city.
    stride::City *m_city;

    ///< List of commuters of the city.
    QList<QTCommuter *> m_commuting_lst;

    ///< Clicked state of the city.
    bool m_is_clicked{false};

    ///< Pointer to te parent back End.
    QTBackEnd *m_back_end;

    ///< Integer of the id (faster lookup).
    const int m_id;

    ///< Population of the city.
    const int m_pop;

};

}
}
