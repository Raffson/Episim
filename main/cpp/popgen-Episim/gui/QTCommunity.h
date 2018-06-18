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
class QTCommunity {
Q_OBJECT

public:
    QTCommunity(Community* model_community);

private:
    Community* m_community;
};



}
}
