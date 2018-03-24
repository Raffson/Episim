//
// Created by beau on 3/5/18.
//

#ifndef STRIDE_HOUSEHOLD_H
#define STRIDE_HOUSEHOLD_H

#include <cstddef>
#include <vector>

using namespace std;

struct Person{
    int age;
};

class Household
{
public:
    Household();

    /// Add a member to the household.
    void AddMember(Person member);

    /// Id of city where housholds lives.
    size_t GetCityID() const { return m_city_id; }

    /// Household Id.
    size_t GetID() const { return m_id; }

    /// Id's of members of household.
    vector<Person> GetMembers() const { return m_members; }

    /// Number of members of the household.
    size_t GetSize() const { return m_members.size(); }

    /// Set Id for city where household lives.
    void SetCityID(size_t city_id);

    void GetSchoolAttendants(vector<Person>&);

private: // Raphael@everyone replace size_t by unsigned int?
    /// Id generator.
    static size_t g_id_generator;

    /// A unique ID of the household.
    size_t              m_id;

    /// A vector of ID's referring to the ID of a person.
    // Raphael@everyone shouldn't we use a vector of shared pointers to Person?
    vector<Person> m_members;

    ///< The ID of the city in which the household is located
    size_t              m_city_id;
};


#endif //STRIDE_HOUSEHOLD_H
