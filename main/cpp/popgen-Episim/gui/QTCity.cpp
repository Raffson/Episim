//
// Created by robbe on 04.06.18.
//

#include "QTCity.h"
#include <algorithm>
#include <boost/range/adaptor/reversed.hpp>

namespace stride {
namespace gui {

QTCity::QTCity(stride::City *model, QTBackEnd *back_end, QObject *parent) :
        QObject(parent), m_city(model), m_commuting_lst(),
        m_back_end(back_end), m_id(model->GetId()),
        m_pop(model->GetEffectivePopulation()) {}


QTCity::QTCity(const QTCity &obj) : QObject(obj.parent()), m_city(obj.GetModelCity()), m_commuting_lst(obj.m_commuting_lst),
                                    m_back_end(obj.GetBackEnd()), m_id(obj.GetId()), m_pop(obj.GetModelCity()->GetEffectivePopulation()) {}


QGeoCoordinate QTCity::GetCoordinates() const {
    return QGeoCoordinate(m_city->GetCoordinates().GetLatitude(), m_city->GetCoordinates().GetLongitude());
}

void QTCity::Flip() {
    if (m_is_clicked) {
        m_back_end->AddSelectedPop(m_pop * -1);
    } else {
        m_back_end->AddSelectedPop(m_pop);
    }
    m_is_clicked = !m_is_clicked;
    emit ClickedChanged();
}

QString QTCity::GetName() const {
    return QString(m_city->GetName().c_str());
}

int QTCity::GetId() const {
    return m_id;
}


void QTCity::SetClicked(bool val) {
    m_is_clicked = val;
    if (val) {
        m_back_end->AddSelectedPop(m_pop);
        m_back_end->AddCommuteLines(m_commuting_lst);
    } else {
        m_back_end->AddSelectedPop(m_pop * -1);
        m_back_end->RemoveCommuteLines(m_commuting_lst);
    }
    emit ClickedChanged();
    emit m_back_end->SelectedInfectedChanged();
}

void QTCity::CreateCommutingList(int amount) {
    // First we sort the outcommuters from small to big
    vector<pair<double, unsigned int>> tmp_vec;
    for (auto &it: m_city->GetOutCommuting()) {
        tmp_vec.emplace_back(make_pair(it.second, it.first));
    }
    std::sort(tmp_vec.begin(), tmp_vec.end());

    //We make a search map so we can find the objects more easly.
    map<unsigned int, QTCity *> search_map;
    for (auto &it: m_back_end->GetCities()) {
        auto cty = dynamic_cast<QTCity *>(it);
        search_map[(unsigned int) cty->GetId()] = cty;

    }

    map<unsigned int, QTCommuter *> found_map; // Map for ones we already have
    // We add amount of outcommuting lines
    for (int i = 0; i < amount; i++) {
        int id = tmp_vec[tmp_vec.size() - 1 - i].second;
        double outcommuters = tmp_vec[tmp_vec.size() - 1 - i].first;

        QTCity *cty2 = search_map.at((unsigned int) id);
        search_map.erase((unsigned int) id); // erase found ones

        auto cmt = new QTCommuter(this, cty2, (int) outcommuters, 0);
        found_map[id] = cmt;
        m_commuting_lst.append(cmt);

    }

    //Sorting the inccomuter as we sorted outcommuters
    vector<pair<double, unsigned int>> tmp_vec2;
    for (auto &it: m_city->GetInCommuting()) {
        tmp_vec2.emplace_back(make_pair(it.second, it.first));
    }
    std::sort(tmp_vec2.begin(), tmp_vec2.end());


    // Now we add the incommuters
    for (int i = 0; i < amount; i++) {
        unsigned int id = tmp_vec2[tmp_vec.size() - 1 - i].second;
        double incommuters = tmp_vec2[tmp_vec.size() - 1 - i].first;

        if (found_map.count(id) ==
            0) { // Line not yet exists bcz we delete found cities in the outcommuters part
            QTCity *cty2 = search_map.at(id);
            auto *cmt = new QTCommuter(this, cty2, 0, (int) incommuters);
            m_commuting_lst.append(cmt);
        } else {

            found_map[id]->set_in_commuters(incommuters);
        }


    }

}
}
}

