//
// Created by beau on 3/5/18.
//

#ifndef STRIDE_HOUSEHOLD_H
#define STRIDE_HOUSEHOLD_H

#include <cstddef>
#include <vector>

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
    std::size_t GetCityID() const { return m_city_id; }

    /// Household Id.
    std::size_t GetID() const { return m_id; }

    /// Id's of members of household.
    std::vector<Person> GetMembers() const { return m_members; }

    /// Number of members of the household.
    std::size_t GetSize() const { return m_members.size(); }

    /// Set Id for city where household lives.
    void SetCityID(std::size_t city_id);

private:
    /// Id generator.
    static std::size_t g_id_generator;

    /// A unique ID of the household.
    std::size_t              m_id;

    /// A vector of ID's referring to the ID of a person.
    std::vector<Person> m_members;

    ///< The ID of the city in which the household is located
    std::size_t              m_city_id;
};


#endif //STRIDE_HOUSEHOLD_H
