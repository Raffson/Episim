//
// Created by robbe on 13.06.18.
//

#include "QTCommuter.h"


QTCommuter::QTCommuter(QTCity* city1, QTCity* city2, int outcommuting, int incommuting,  QObject* parent ):
        QObject(parent),m_city1(city1), m_city2(city2), m_center_city1(city1->get_coordinates()),
        m_center_city2(city2->get_coordinates()),m_out_count(outcommuting), m_in_count(incommuting){

}
/*QTCommuter::QTCommuter(const QTCommuter& obj):QObject(obj.parent()), m_city1(obj.m_city1), m_city2(obj.m_city2),
                                              m_center_city1(obj.m_center_city1), m_center_city2(obj.m_center_city2),
                                              m_out_count(obj.m_out_count), m_in_count(obj.m_in_count)
{

}*/


void QTCommuter::set_in_commuters(int amount) {
        m_in_count = amount;
}

QTCity *QTCommuter::get_main_city() {
        return m_city1;
}

