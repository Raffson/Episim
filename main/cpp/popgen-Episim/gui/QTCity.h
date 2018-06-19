//
// Created by robbe on 04.06.18.
// Contains the QTCity wrapper to use Cities in QT QML.
//

#pragma once

#include "popgen-Episim/model/City.h"
#include "QTBackEnd.h"
#include "QTCommuter.h"
#include "QTCommunity.h"
#include "QTHouseHold.h"

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
    explicit QTCity(stride::City *m_city = nullptr, QTBackEnd *back_end =nullptr);

    /// @brief Copy constructor. Makes a non trivial deep copy of QTCity.
    /// @param object: QTCity to be copied.
    QTCity(const QTCity &object);
    QTCity& operator=(const QTCity&)=delete;

///@}

/// Q Properties
/***********************************************************************************************************************/
/**
* @name: QProperties
* Properties available to the City Qml type.
*/
///@{
    ///< The coordinate of our city center.
    Q_PROPERTY(QGeoCoordinate crd READ GetCoordinates CONSTANT)

    ///< population amount in city
    Q_PROPERTY(int popCount MEMBER m_pop CONSTANT)

    ///< name of the city
    Q_PROPERTY(QString name READ GetName CONSTANT)

    ///< id of the city
    Q_PROPERTY(int id READ GetId CONSTANT)

    ///< total commuters into the city
    Q_PROPERTY(int total_in_commuters READ GetTotalCommutersIn CONSTANT)

    ///< total commuters out of the city
    Q_PROPERTY(int total_out_commuters READ GetTotalCommutersOut CONSTANT)

    ///< infected in the city
    Q_PROPERTY(int infected READ GetInfected NOTIFY InfectedChanged)

    ///< clicked state of the city
    Q_PROPERTY(bool clicked READ IsClicked WRITE SetClicked NOTIFY ClickedChanged)

    ///< List of all schools in city
    Q_PROPERTY(QList<QObject*> schools MEMBER m_schools NOTIFY CommunitiesChanged)

    ///< List of all colleges in city
    Q_PROPERTY(QList<QObject*> colleges MEMBER m_colleges NOTIFY CommunitiesChanged)

    ///< List of al workplaces in city
    Q_PROPERTY(QList<QObject*> workplaces MEMBER m_workplaces NOTIFY CommunitiesChanged)

    ///< List of al primary communities
    Q_PROPERTY(QList<QObject*> primary_communities MEMBER m_primary NOTIFY CommunitiesChanged)

    ///< List of all secondary communities
    Q_PROPERTY(QList<QObject*> secondary_communities MEMBER m_secondary NOTIFY CommunitiesChanged)

    ///< List of all Households
    Q_PROPERTY(QList<QObject*> households MEMBER m_households NOTIFY HouseHoldsChanged())
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

    ///@brief Notifies QMl Communitylists changed
    void CommunitiesChanged();

    ///@brief Notifies QML Households changed
    void HouseHoldsChanged();
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

//Helper functions
/***********************************************************************************************************************/
private:
///@{
    /// @brief Will make wrappers for the communities in the cities
    void CreateCommunityLists();

    /// @brief Will fill in the list of given type
    /// @param the type as CommunityType
    /// @return returns a list of Qobjects of the given type.
    QList<QObject *> CreateCommunityList(CommunityType::Id id);

    /// @brief creates the list of QT wrappers for this city.
    void CreateHouseHoldList();
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

    ///< List of schools
    QList<QObject*> m_schools{};

    ///< List of colleges
    QList<QObject*> m_colleges{};

    ///< List of workplaces
    QList<QObject*> m_workplaces{};

    ///< List of primary communities
    QList<QObject*> m_primary{};

    ///< List of secondary communities
    QList<QObject*> m_secondary{};

    ///< List of households
    QList<QObject*> m_households{};

};

}
}
