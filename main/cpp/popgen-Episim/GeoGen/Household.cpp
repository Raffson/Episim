//
// Created by beau on 3/5/18.
//

#include "Household.h"

using namespace std;
using namespace stride;

namespace stride {

unsigned int Household::m_id_generator = 1;

Household::Household(City* city)
    : m_id(m_id_generator++), m_city(city), m_pool(ContactPool(m_id, ContactPoolType::Id::Household))
{
}

void Household::AddMember(const Person* member) { m_pool.AddMember(member); }

void Household::GetSchoolAttendants(vector<Person*>& vec) const
{
        for (auto a_member : m_pool.GetPool()) {
                if (a_member->GetAge() < 18 and a_member->GetAge() >= 3) {
                        vec.emplace_back(a_member);
                }
        }
}

void Household::GetPossibleStudents(vector<Person*>& vec) const
{
        for (auto a_member : m_pool.GetPool()) {
                if (a_member->GetAge() >= 18 && a_member->GetAge() < 26) {
                        vec.emplace_back(a_member);
                }
        }
}

void Household::GetPossibleWorkers(vector<Person*>& vec) const
{
        for (auto a_member : m_pool.GetPool()) {
                if (a_member->GetAge() >= 18 && a_member->GetAge() <= 64) {
                        vec.emplace_back(a_member);
                }
        }
}

} // namespace stride
