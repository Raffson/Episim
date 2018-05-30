#pragma once
//
// Created by beau on 3/5/18.
//

#include "pop/Person.h"
#include "pool/ContactPoolSys.h"
#include <vector>
#include "omp.h"

using namespace std;

namespace stride {

class City;
class Person;

class Household
{
public:
        /// No default constructor.
        Household() = delete;

        /// Household's contructor.
        /// @param city A pointer to the city to which this household's assigned.
        /// @param pool A reference to the ContactPoolSys used by stride to create the contact pool
        Household(City* city, ContactPoolSys& pool_sys);

        /// Add a member to the household.
        /// @param: member the person to be added to the household
        void AddMember(const Person* member);

        /// A const reference to the city where this household is located.
        const City* GetCity() const { return m_city; }

        /// A reference to the city where this household is located.
        City* GetCity() { return m_city; }

        /// Household Id.
        size_t GetID() const { return m_id; }

        /// Return the members of this household.
        const vector<Person*>& GetMembers() const { return m_pool->GetPool(); }

        /// Number of members of the household.
        size_t GetSize() const { return m_pool->GetSize(); }

        // do we need this?
        void GetSchoolAttendants(vector<Person*>&) const;

        void GetPossibleStudents(vector<Person*>&) const;

        void GetPossibleWorkers(vector<Person*>&) const;

private:
        const size_t m_id;   ///< A unique ID of the household.
        City*        m_city; ///< The City in which the household is located
        ContactPool* m_pool; ///< A ContactPool with the people belonging to this household.

};

} // namespace stride
