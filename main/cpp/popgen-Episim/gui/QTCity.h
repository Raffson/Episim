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
* @name: Constructors, Destructors.
*/
    ///@{
    /// @brief constructor of QCity. Note: could be used as QTCity() default so it can be a MetaObject in QML.
    /// @param m_city: the city model that corresponds to this QTCity
    /// @param back_end: pointer to the back_end that is a parent of QTCity
    /// @param parent: Qobject parent: is used in QML, we use it to call QObject constructor.
    explicit QTCity(stride::City *m_city = nullptr, QTBackEnd *back_end = nullptr, QObject *parent = nullptr);

    /// @brief Copy constructor. Makes a non trivial deep copy of QTCity
    /// @param object QTCity to be copied
    QTCity(const QTCity &object);

    /// @brief Default destructor
    ~QTCity() override = default;
///@}

/// Q Properties
/***********************************************************************************************************************/
/**
* @name: QProperties
* Properties available to the City Qml type
*/
///@{
    ///< The coordinate of our city center.
    Q_PROPERTY(QGeoCoordinate crd
                       READ
                               GetCoordinates
                       CONSTANT)

    ///< population amount in city
    Q_PROPERTY(int popCount
                       MEMBER
                               m_pop
                       CONSTANT)

    ///< name of the city
    Q_PROPERTY(QString name
                       READ
                               GetName
                       CONSTANT)

    ///< id of the city
    Q_PROPERTY(int id
                       READ
                               GetId
                       CONSTANT)

    ///< total commuters into the city
    Q_PROPERTY(int total_in_commuters
                       READ
                               GetTotalCommutersIn
                       CONSTANT)

    ///< total commuters out of the city
    Q_PROPERTY(int total_out_commuters
                       READ
                               GetTotalCommutersOut
                       CONSTANT)

    ///< infected in the city
    Q_PROPERTY(int infected
                       READ
                               GetInfected
                       NOTIFY
                       InfectedChanged)

    ///< clicked state of the city
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
* all signals of QTCity
*/
    ///@{
    ///@brief notifies QML that the infected changed
    void InfectedChanged();

    ///@brief Notifies QML that the clicked state changed
    void ClickedChanged();
///@}

public:
// Retrieval
/***********************************************************************************************************************/
/**
* @name Retrieval
* All the getters of City
*/
    ///@{
    ///@brief returns a poiner to the model city
    ///@return A City* to the model city.
    stride::City *GetModelCity() const { return m_city; }

    ///@brief returns a pointer to the back end.
    ///@return returns a pointer to the back end cbject that is the parent of this city.
    QTBackEnd *GetBackEnd() const { return m_back_end; }

    ///@brief returns the infected count in the city
    ///@return A integer that represents the infected count of this city.
    int GetInfected() const { return (int) m_city->GetInfectedCount(); }

    ///@brief returns the clicked state of our city
    ///@return a bool representing the clicked state of the city.
    bool IsClicked() const { return m_is_clicked; }

    ///@brief returns the id of the city.
    ///@return an integer representing the id of the city.
    int GetId() const;

    ///@brief returns the coordinates of the city.
    ///@returns A QGeocoordinate representing the coordinate of the city in latitude and longitude.
    QGeoCoordinate GetCoordinates() const;

    ///@brief returns the commuters of the city
    ///@return a list of QTCommuters representing our commuter lines.
    const QList<QTCommuter *> &GetCommuters() const { return m_commuting_lst; }

    ///@brief returns the name of the city
    ///@return a Qstring with the name of the city
    QString GetName() const;

    ///@brief returns a integer value of the outcommuters
    ///@return a integer value of totalOutCommuters.
    int GetTotalCommutersOut() { return (int) m_city->GetTotalOutCommutersCount(); }

    ///@brief returns a integer value of the incommuters.
    ///@return returns a ingerer value of the incommuters
    int GetTotalCommutersIn() { return (int) m_city->GetTotalInCommutersCount(); }
///@}

// Modifiers
/************************************************************************************************************************/
/**
* @name modifiers
* All the modifiers of city
*/
///@{
    /// @brief flips the state of our city between clicked/not cicked
    void Flip();

    /// @brief sets the state of our city and handles all emits nescesary.
    /// @param a bool representing the new clicked state clicked = true/false.
    void SetClicked(bool val);

    ///@brief creates the commuting list. Note that this has to happen after ALL QTCities are created otherwise
    ///       we try to refer not yet existing cities.
    ///@param amount the amount of in and out commuters to be made
    void CreateCommutingList(int amount);
///@}

// Data members
/***********************************************************************************************************************/
private:
/**
* @name Datamembers
* the data members
*/
    ///@{
    ///< Pointer to the model city
    stride::City *m_city;

    ///< List of commuters of the city
    QList<QTCommuter *> m_commuting_lst;

    ///< Clicked state of the city
    bool m_is_clicked{false};

    ///< Pointer to te parent back End
    QTBackEnd *m_back_end;

    /// integer of the id (faster lookup)
    const int m_id;

    /// population of the city
    const int m_pop;

};

}
}
