//
// Created by beau on 3/5/18.
//

#include "Household.h"
#include <iostream>

using namespace std;

size_t Household::g_id_generator = 1;

Household::Household()
{
    m_id = g_id_generator;
    g_id_generator++;
}

void Household::AddMember(Person member) {
    m_members.push_back(member);
}

void Household::SetCityID(size_t city_id) {
    m_city_id = city_id;
}

void Household::GetSchoolAttendants(vector<Person>& vec) {

    for(auto a_member:m_members){
        if(a_member.age < 18){
            vec.push_back(a_member);
        }
    }
}
