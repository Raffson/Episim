//
// Created by beau on 3/5/18.
//

#include "Household.h"

using namespace std;
using namespace stride;

size_t Household::g_id_generator = 1;

Household::Household(size_t city_id) : m_id(g_id_generator), m_city_id(city_id),
                                       m_pool(make_shared<ContactPool>(m_id, ContactPoolType::Id::Household))
{
        g_id_generator++;
}

void Household::AddMember(const Person* member) { m_pool->AddMember(member); }

//void Household::SetCityID(size_t city_id) { m_city_id = city_id; }


void Household::GetSchoolAttendants(vector<Person*>& vec) const
{
        for (auto a_member : m_pool->GetPool()) {
                if (a_member->GetAge() < 18 and a_member->GetAge() >= 3) {
                        vec.emplace_back(a_member);
                }
        }
}

void Household::GetPossibleStudents(vector<Person*>& vec) const
{
        for (auto a_member : m_pool->GetPool()) {
                if (a_member->GetAge() >= 18 && a_member->GetAge() < 26) {
                        vec.emplace_back(a_member);
                }
        }
}

void Household::GetPossibleWorkers(vector<Person*>& vec) const
{
        for (auto a_member : m_pool->GetPool()) {
                if (a_member->GetAge() >= 18 && a_member->GetAge() <= 64) {
                        vec.emplace_back(a_member);
                }
        }
}
