//
// Created by beau on 3/5/18.
//

#include "Household.h"

using namespace std;

namespace stride {

Household::Household(City* city, ContactPoolSys& pool_sys) :
        m_id(pool_sys[ContactPoolType::Id::Household].size()+1), m_city(city)
{
        ContactPoolType::Id type = ContactPoolType::Id::Household;
        pool_sys[type].emplace_back(m_id, type, this);
        m_pool = &pool_sys[type].back();
}

void Household::AddMember(const Person* member) { m_pool->AddMember(member); }

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

} // namespace stride
