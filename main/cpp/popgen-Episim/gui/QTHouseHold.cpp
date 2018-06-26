//
// Created by robbe on 19.06.18.
//

#include "QTHouseHold.h"


namespace stride{
namespace gui{

QTHouseHold::QTHouseHold(Household* model_ptr, QObject* parent): QObject(parent), m_household(model_ptr){}

}
}