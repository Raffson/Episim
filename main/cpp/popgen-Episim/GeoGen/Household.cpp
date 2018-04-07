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

void Household::AddMember(const shared_ptr<stride::Person>& member) { m_members.push_back(member); }

void Household::SetCityID(size_t city_id) { m_city_id = city_id; }

void Household::GetSchoolAttendants(vector<shared_ptr<stride::Person>>& vec) const
{

        for (auto a_member : m_members) {
                if (a_member.GetAge() < 18 and a_member.GetAge() >= 3) {
                        vec.push_back(a_member);
                }
        }
}

void Household::GetPossibleStudents(vector<shared_ptr<stride::Person>>& vec) const
{
        for (auto a_member : m_members) {
                if (a_member.GetAge() >= 18 && a_member.GetAge() < 26) {
                        vec.push_back(a_member);
                }
        }
}

void Household::GetPossibleWorkers(vector<shared_ptr<stride::Person>>& vec) const
{
        for (auto a_member : m_members) {
                if (a_member.GetAge() >= 18 && a_member.GetAge() <= 64) {
                        vec.push_back(a_member);
                }
        }
}
