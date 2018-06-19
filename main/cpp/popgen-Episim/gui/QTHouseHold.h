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
///@{
    /// @brief Constructor
    /// @param model_household The model household this QTHousehold wraps around
    /// @param optional
    explicit QTHouseHold(Household* model_household, QObject* parent = nullptr);

    QTHouseHold(const QTHouseHold& object) = delete;
    QTHouseHold& operator = (const QTHouseHold& object) = delete;
///@}

private:
    Household* m_household;
};

}
}


