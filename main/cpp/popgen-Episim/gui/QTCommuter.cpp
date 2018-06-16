//
// Created by robbe on 13.06.18.
//

#include <algorithm>
#include "QTCommuter.h"
#include <iostream>
using namespace std;

QTCommuter::QTCommuter(QTCity* city1, QTCity* city2, double outcommuting, double incommuting,  QObject* parent ):
        QObject(parent),m_city1(city1), m_city2(city2), m_center_city1(city1->GetCoordinates()),
        m_center_city2(city2->GetCoordinates()),m_out_count(outcommuting), m_in_count(incommuting){

    cout << m_center_city1.latitude() <<" "  << m_center_city1.latitude() << ", " <<  m_center_city2.latitude() << m_center_city2.longitude() << endl ;

}
/*QTCommuter::QTCommuter(const QTCommuter& obj):QObject(obj.parent()), m_city1(obj.m_city1), m_city2(obj.m_city2),
                                              m_center_city1(obj.m_center_city1), m_center_city2(obj.m_center_city2),
                                              m_out_count(obj.m_out_count), m_in_count(obj.m_in_count)
{

}*/


void QTCommuter::set_in_commuters(double amount) {
        m_in_count = amount;
}

QTCity *QTCommuter::GetMainCity() {
        return m_city1;
}
QTCity *QTCommuter::GetSecondCity() {
    return m_city2;
}


double QTCommuter::CalculateLineWidth() {

    double biggest = max(m_in_count, m_out_count);
    double biggest_total = max(m_city1->GetModelCity()->GetTotalInCommutersCount(),
                               m_city1->GetModelCity()->GetTotalOutCommutersCount());

    return  biggest / biggest_total * 50;
}

QColor QTCommuter::PickColor() {

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


