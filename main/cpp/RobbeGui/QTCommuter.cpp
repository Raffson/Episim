//
// Created by robbe on 13.06.18.
//

#include "QTCommuter.h"


QTCommuter::QTCommuter(QTCity* city1, QTCity* city2, int outcommuting, int incommuting,  QObject* parent ):
        QObject(parent),m_city1(city1), m_city2(city2), m_out_count(outcommuting), m_in_count(incommuting){

}