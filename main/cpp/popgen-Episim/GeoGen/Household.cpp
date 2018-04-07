//
// Created by beau on 3/5/18.
//

#include "Household.h"

using namespace std;

size_t Household::g_id_generator = 1;

Household::Household(size_t city_id) : m_id(g_id_generator), m_city_id(city_id),
                                       m_pool(make_shared<stride::ContactPool>(m_id, stride::ContactPoolType::Id::Household))
{
        g_id_generator++;
}

void Household::AddMember(const shared_ptr<stride::Person>& member) { }//m_pool->AddMember(member); }

//void Household::SetCityID(size_t city_id) { m_city_id = city_id; }

vector<shared_ptr<const stride::Person>> Household::GetMembers() const
{
    vector<shared_ptr<const stride::Person>> result;
    for (auto a_member : m_pool->GetPool()) {
        result.emplace_back(make_shared<const stride::Person>(*a_member));
    }
    return result;
}

void Household::GetSchoolAttendants(vector<shared_ptr<const stride::Person>>& vec) const
{
        for (auto a_member : m_pool->GetPool()) {
                if (a_member->GetAge() < 18 and a_member->GetAge() >= 3) {
                        vec.emplace_back(a_member);
                }
        }
}

void Household::GetPossibleStudents(vector<shared_ptr<const stride::Person>>& vec) const
{
        for (auto a_member : m_pool->GetPool()) {
                if (a_member->GetAge() >= 18 && a_member->GetAge() < 26) {
                        vec.emplace_back(a_member);
                }
        }
}

void Household::GetPossibleWorkers(vector<shared_ptr<const stride::Person>>& vec) const
{
        for (auto a_member : m_pool->GetPool()) {
                if (a_member->GetAge() >= 18 && a_member->GetAge() <= 64) {
                        vec.emplace_back(a_member);
                }
        }
}
