//
// Created by robbe on 18.06.18.
//
#pragma once

#include <QObject>
#include "popgen-Episim/model/Community.h"

namespace stride {
namespace gui {

/**
 * Wrapper for the community class by QT
 */
class QTCommunity: public QObject {
Q_OBJECT

public:
// Constructors, Destructor
/***********************************************************************************************************************/
    /// @brief constructor, note that the parent is not optional and is a QTCity.
    /// @param the community model
    /// @param the parent in this case a city
    explicit QTCommunity(Community *model_community, QObject* parent);

    /// @brief copy constructor, shallow copy.
    QTCommunity(const QTCommunity& object);

// Properties
//**********************************************************************************************************************/
    Q_PROPERTY(int id READ GetId CONSTANT)
    Q_PROPERTY(int pop MEMBER m_population NOTIFY PopChanged())

//Signals
//**********************************************************************************************************************/
signals:
    void PopChanged();

public:
// Operators
/***********************************************************************************************************************/
    ///@brief assignment shallow assignment
    QTCommunity& operator = (const QTCommunity& object);


//Getters
/***********************************************************************************************************************/
    int GetId(){return (int) m_community->GetID();};

private:
    void CountPopInfected();


private:
    Community* m_community;
    //int m_infected{0};
    int m_population{0};
};



}
}
