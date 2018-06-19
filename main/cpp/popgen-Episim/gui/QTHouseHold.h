//
// Created by robbe on 19.06.18.
//

#pragma once

#include <QObject>
#include "popgen-Episim/model/Household.h"

namespace stride{
namespace gui{

/***
 * Represents a QTHousehold objct in QML. Acts as a wrapper for a household.
 */
class QTHouseHold: public QObject {
Q_OBJECT

public:
// Constructor, copy and assignment
///*********************************************************************************************************************/
/**
 * @name Constructor, copy and assignment
 */
///@{
    /// @brief Constructor.
    /// @param model_household: The model household this QTHousehold wraps around.
    /// @param parent: optional argument.
    explicit QTHouseHold(Household* model_household, QObject* parent = nullptr);

    QTHouseHold(const QTHouseHold& object) = delete;
    QTHouseHold& operator = (const QTHouseHold& object) = delete;
///@}


/// Properties
/************************************************************************************************************************/
        ///> pop: Property of popularion.
        Q_PROPERTY(int pop READ CountPop CONSTANT)
        ///> id: Property of id.
        Q_PROPERTY(int id READ GetId CONSTANT)


public:
/// Getters
/***********************************************************************************************************************/
    /// @brief Counts the size of the model.
    int CountPop(){return (int)m_household->GetSize();}

    /// @brief Returns the id of household.
    int GetId(){return (int) m_household->GetID();}
private:
    Household* m_household;

};

}
}


