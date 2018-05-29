//
// Created by beau on 3/5/18.
//

#include "Household.h"

using namespace std;

namespace stride {

Household::Household(City* city, ContactPoolSys& poolSys) :
        m_id(poolSys[ContactPoolType::Id::Household].size()+1), m_city(city)
{
        ContactPoolType::Id type = ContactPoolType::Id::Household;
        poolSys[type].emplace_back(m_id, type, this);
        m_pool = &poolSys[type].back();
}

void Household::AddMember(const Person* member) {
#pragma omp critical(add_member)
        {
                m_pool->AddMember(member);
        }
}

void Household::GetSchoolAttendants(vector<Person*>& vec) const
{
        for (auto aMember : m_pool->GetPool()) {
                if (aMember->GetAge() < 18 and aMember->GetAge() >= 3) {
                        vec.emplace_back(aMember);
                }
        }
}

void Household::GetPossibleStudents(vector<Person*>& vec) const
{
        for (auto aMember : m_pool->GetPool()) {
                if (aMember->GetAge() >= 18 && aMember->GetAge() < 26) {
                        vec.emplace_back(aMember);
                }
        }
}

void Household::GetPossibleWorkers(vector<Person*>& vec) const
{
        for (auto aMember : m_pool->GetPool()) {
                if (aMember->GetAge() >= 18 && aMember->GetAge() <= 64) {
                        vec.emplace_back(aMember);
                }
        }
}

} // namespace stride
