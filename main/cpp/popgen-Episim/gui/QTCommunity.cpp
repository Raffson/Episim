//
// Created by robbe on 18.06.18.
//

#include "QTCommunity.h"

namespace stride {
namespace gui {


QTCommunity::QTCommunity(Community *model_community, QObject *parent):QObject(parent),m_community(model_community){
    CountPopInfected();
}

QTCommunity::QTCommunity(const stride::gui::QTCommunity &object) :QObject(object.parent()), m_community(object.m_community){
    CountPopInfected();
}

QTCommunity& stride::gui::QTCommunity::operator = (const QTCommunity& object) {
    m_community = object.m_community;
    setParent(object.parent());
    return *this;
}

void QTCommunity::CountPopInfected() {
    for(auto& contactpool: m_community->GetContactPools()){
            m_population += contactpool->GetSize();
    }

    emit PopChanged();

}


}
}