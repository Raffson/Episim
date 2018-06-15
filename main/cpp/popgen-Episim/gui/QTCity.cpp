//
// Created by robbe on 04.06.18.
//

#include "QTCity.h"
#include <algorithm>
#include <boost/range/adaptor/reversed.hpp>



QTCity::QTCity(stride::City* model,QTBackEnd* back_end, QObject *parent ):
        QObject(parent), m_city(model),m_back_end(back_end), m_id(model->GetId()), m_pop(model->GetEffectivePopulation()){
    fill_in_in_commuters();
    fill_in_out_commuters();
}

QTCity::QTCity(const QTCity& obj): QObject(obj.parent()), m_city(obj.get_m_city()), m_back_end(obj.get_back_end()),
                                    m_id(obj.get_id()), m_pop(obj.get_m_city()->GetEffectivePopulation()){

    fill_in_in_commuters();
    fill_in_out_commuters();

}



QGeoCoordinate QTCity::get_coordinates() const {
    return QGeoCoordinate(m_city->GetCoordinates().GetLatitude(), m_city->GetCoordinates().GetLongitude());
}

void QTCity::flip(){

    m_is_clicked = !m_is_clicked;
    emit clickedChanged();
}

QString QTCity::get_name() const {
    return QString(m_city->GetName().c_str());
}

int QTCity::get_id() const {
    return m_id;
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

void QTCity::set_clicked(bool val){
    m_is_clicked = val;
    if(val){
        m_back_end->add_selected_pop(m_pop);
        m_back_end->add_commute_lines(m_commuting_lst);
    }
    else{
        m_back_end->add_selected_pop(m_pop  * -1);
        m_back_end->remove_commute_lines(m_commuting_lst);
    }
    emit clickedChanged();
    emit m_back_end->selected_infectedChanged();
}

void QTCity::create_commuting_lst(int amount){
    // First we sort the outcommuters from small to big
    vector<pair<double, unsigned int>> tmp_vec;
    for(auto& it: m_city->GetOutCommuting()){
        tmp_vec.emplace_back(make_pair(it.second, it.first));
    }
    std::sort(tmp_vec.begin(), tmp_vec.end());

    //We make a search map so we can find the objects more easly.
    map<unsigned int, QTCity*> search_map;
    for(auto& it: m_back_end->get_cities()) {
        auto cty = dynamic_cast<QTCity*>(it);
        search_map[(unsigned int)cty->get_id()] = cty;

    }

    map<unsigned int, QTCommuter*> found_map; // Map for ones we already have
    // We add amount of outcommuting lines
    for(int i = 0; i < amount; i++){
        int id = tmp_vec[tmp_vec.size() -1 - i].second;
        double outcommuters = tmp_vec[tmp_vec.size() -1 - i].first;

        QTCity* cty2 = search_map.at((unsigned int) id);
        search_map.erase((unsigned int)id); // erase found ones

        auto cmt = new QTCommuter(this, cty2,(int)outcommuters, 0);
        found_map[id] = cmt;
        m_commuting_lst.append(cmt);

    }

    //Sorting the inccomuter as we sorted outcommuters
    vector<pair<double, unsigned int>> tmp_vec2;
    for(auto& it: m_city->GetInCommuting()){
        tmp_vec2.emplace_back(make_pair(it.second, it.first));
    }
    std::sort(tmp_vec2.begin(), tmp_vec2.end());


    // Now we add the incommuters
    for(int i = 0; i < amount; i++){
        unsigned int id = tmp_vec2[tmp_vec.size() -1 - i].second;
        double incommuters = tmp_vec2[tmp_vec.size() -1 - i].first;

        if(found_map.count(id) == 0){ // Line not yet exists bcz we delete found cities in the outcommuters part
            QTCity *cty2 = search_map.at(id);
            auto* cmt = new QTCommuter(this, cty2,0, (int) incommuters);
            m_commuting_lst.append(cmt);
        }
        else {

            found_map[id]->set_in_commuters(incommuters);
            }


    }

}

