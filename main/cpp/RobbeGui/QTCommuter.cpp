//
// Created by robbe on 13.06.18.
//

#include <algorithm>
#include "QTCommuter.h"
#include <iostream>
using namespace std;

QTCommuter::QTCommuter(QTCity* city1, QTCity* city2, double outcommuting, double incommuting,  QObject* parent ):
        QObject(parent),m_city1(city1), m_city2(city2), m_center_city1(city1->get_coordinates()),
        m_center_city2(city2->get_coordinates()),m_out_count(outcommuting), m_in_count(incommuting){

}
/*QTCommuter::QTCommuter(const QTCommuter& obj):QObject(obj.parent()), m_city1(obj.m_city1), m_city2(obj.m_city2),
                                              m_center_city1(obj.m_center_city1), m_center_city2(obj.m_center_city2),
                                              m_out_count(obj.m_out_count), m_in_count(obj.m_in_count)
{

}*/


void QTCommuter::set_in_commuters(double amount) {
    cout << amount << endl;
        m_in_count = amount;
}

QTCity *QTCommuter::get_main_city() {
        return m_city1;
}

double QTCommuter::calculate_line_width() {

    double biggest = max(m_in_count, m_out_count);
    double biggest_total = max(m_city1->get_m_city()->GetTotalInCommutersCount(),
            m_city1->get_m_city()->GetTotalOutCommutersCount());

    return  biggest / biggest_total * 50;
}

QColor QTCommuter::pick_color() {

    if(m_in_count == 0){ // so it is an outcommuters line

        return QColor("green");
    }
    else if(m_out_count == 0){
        return QColor("red");
    }
    else{
        return QColor("blue");
    }
}

