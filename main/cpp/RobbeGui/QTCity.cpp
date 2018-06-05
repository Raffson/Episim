//
// Created by robbe on 04.06.18.
//

#include "QTCity.h"
#include <algorithm>
#include <boost/range/adaptor/reversed.hpp>

QTCity::QTCity(stride::City* model, QObject *parent ) : QObject(parent), m_city(model){
    fill_in_in_commuters();
    fill_in_out_commuters();
}

QTCity::QTCity(QObject *parent ) : QObject(parent){

}

int QTCity::get_population() const {
    return m_city->GetEffectivePopulation();
}

QGeoCoordinate QTCity::get_coordinates() const {
    return QGeoCoordinate(m_city->GetCoordinates().GetLatitude(), m_city->GetCoordinates().GetLongitude());
}

QString QTCity::get_name() const {
    return QString(m_city->GetName().c_str());
}

int QTCity::get_id() const {
    return m_city->GetId();
}

void QTCity::fill_in_out_commuters() {

    vector<pair<double, unsigned int>> tmp_vec;
    for(auto& it: m_city->GetOutCommuting()){
        tmp_vec.emplace_back(make_pair(it.second, it.first));
    }

    std::sort(tmp_vec.begin(), tmp_vec.end());

    for(auto& it: boost::adaptors::reverse(tmp_vec)){
        m_sorted_out_commuters.append(it.second);
        m_commuter_out_count.append(it.first);
    }
}

void QTCity::fill_in_in_commuters() {

    vector<pair<double, unsigned int>> tmp_vec;
    for(auto& it: m_city->GetInCommuting()){
        tmp_vec.emplace_back(make_pair(it.second, it.first));
    }

    std::sort(tmp_vec.begin(), tmp_vec.end());

    for(auto& it: boost::adaptors::reverse(tmp_vec)){
        m_sorted_in_commuters.append(it.second);
        m_commuter_in_count.append(it.first);
    }
}
