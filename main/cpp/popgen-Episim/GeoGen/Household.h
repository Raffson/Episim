#pragma once
//
// Created by beau on 3/5/18.
//

#include <cstddef>
#include <memory>
#include <vector>

#include "pool/ContactPoolSys.h"
#include "pop/Person.h"

using namespace std;


// Regarding the choice to use raw pointers,
// this should be better for performance,
// and looking at the way this is handled in the Population class,
// there shouldn't be any real dangers...

// I have a backup where I converted everything related to Person
// so that it uses shared pointers, just in case...

namespace stride {

class City;

class Household
{
public:
        /// Household's contructor.
        /// @param city A pointer to the city to which this household's assigned.
        /// @param pool A reference to the ContactPoolSys used by stride to create the contact pool
        Household(City* city, ContactPoolSys& pool_sys);

        /// Add a member to the household.
        /// @param: member the person to be added to the household
        void AddMember(const Person* member);

        /// City where housholds is located.
        City& GetCity() const { return *m_city; }

        /// Household Id.
        unsigned int GetID() const { return m_id; }

        /// Return the members of this household.
        const vector<Person*>& GetMembers() const { return m_pool->GetPool(); }

        /// Number of members of the household.
        size_t GetSize() const { return m_pool->GetSize(); }

        // Next 3 functions could be made private while declaring PopulationGenerator as a friend class...
        // second thought, after fixing PopulationGenerator I wonder if we need these at all...
        // if we do, they'll need to be slightly reworked...
        void GetSchoolAttendants(vector<Person*>&) const;

        void GetPossibleStudents(vector<Person*>&) const;

        void GetPossibleWorkers(vector<Person*>&) const;

private:
        static unsigned int m_id_generator; ///< Id generator.
        size_t m_id;            ///< A unique ID of the household.
        City* m_city;           ///< The City in which the household is located
        ContactPool* m_pool;    ///< A ContactPool with the people belonging to this household.

};

} // namespace stride
