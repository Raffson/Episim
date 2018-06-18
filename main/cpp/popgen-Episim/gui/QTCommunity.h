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
    explicit QTCommunity(Community *model_community, QObject *parent = nullptr);
    QTCommunity(const QTCommunity& object);


/***********************************************************************************************************************/
    QTCommunity& operator = (const QTCommunity& object);

private:
    Community* m_community;
};



}
}
