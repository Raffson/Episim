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

void Household::AddMember(Person member) { m_members.push_back(member); }

void Household::SetCityID(size_t city_id) { m_city_id = city_id; }

// Raphael@everyone while this approach is faster if this vector 'vec' becomes large,
// households are usually small and thus we might as well return a copy of a vector we make locally...
// question is what would be easiest to use? I say return a copy of the vector instead of
// providing a vector to this function which will be adjusted...
// nevertheless, this approach should always be more efficient...
void Household::GetSchoolAttendants(vector<Person>& vec)
{

        for (auto a_member : m_members) {
                if (a_member.age < 18) {
                        vec.push_back(a_member);
                }
        }
}

void Household::GetCollegeStudents(vector<Person>& vec)
{
        for (auto a_member : m_members) {
                if (a_member.age >= 18 && a_member.age < 26 && a_member.work_id == 0) {
                        vec.push_back(a_member);
                }
        }
}

// Raphael@everyone, aren't we forgetting to exclude students in this function?
// I guess it depends on the interpretation,
// as I mentioned in PopulationGenerator we could have students who also work part-time...

void Household::GetPossibleWorkers(vector<Person>& vec)
{
        for (auto a_member : m_members) {
                if (a_member.age >= 18 && a_member.age <= 64) {
                        vec.push_back(a_member);
                }
        }
}
