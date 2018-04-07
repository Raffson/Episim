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

void Household::GetSchoolAttendants(vector<shared_ptr<stride::Person>>& vec)
{

        /*for (auto a_member : m_members) {
                if (a_member.age < 18) {
                        vec.push_back(a_member);
                }
        }*/
}

void Household::GetCollegeStudents(vector<shared_ptr<stride::Person>>& vec)
{
        /*for (auto a_member : m_members) {
                if (a_member.age >= 18 && a_member.age < 26 && a_member.work_id == 0) {
                        vec.push_back(a_member);
                }
        }*/
}

// Raphael@everyone, aren't we forgetting to exclude students in this function?
// according to Nishchal's answer we can't have part-time students, thus I believe we're forgetting
// to check for a work_id != 0 to ensure we're not taking students along...
void Household::GetPossibleWorkers(vector<shared_ptr<stride::Person>>& vec)
{
        /*for (auto a_member : m_members) {
                if (a_member.age >= 18 && a_member.age <= 64) {
                        vec.push_back(a_member);
                }
        }*/
}
