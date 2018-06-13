//
// Created by robbe on 13.06.18.
//

#include "QTCommuter.h"


QTCommuter::QTCommuter(stride::Coordinate c1, stride::Coordinate c2, QObject* parent ): QObject(parent)
        ,m_c1(c1.GetLatitude(), c1.GetLongitude()), m_c2(c2.GetLatitude(), c2.GetLongitude()){

}