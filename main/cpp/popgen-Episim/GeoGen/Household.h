#pragma once
//
// Created by beau on 3/5/18.
//

#include <cstddef>
#include <vector>
#include <memory>

#include "pop/Person.h"

using namespace std;


//No namespace?
// also wondering if we should just put everything in namespace stride
// instead of working with our own namespaces...

//Regarding the choice to use raw pointers,
// this should be better for performance,
// and looking at the way this is handled in the Population class,
// there shouldn't be any real dangers...

//I have a backup where I converted everything related to Person
// so that it uses shared pointers, just in case...

namespace stride { //temproralily put this in namespace stride cause I say we gotta put everything in stride

class City;

class Household
{
public:
        Household(City* city);

        /// Add a member to the household.
        void AddMember(const Person* member);

        /// City where housholds is located.
        City& GetCity() const { return *m_city; }

        /// Household Id.
        unsigned int GetID() const { return m_id; }

        /// Return the members of this household.
        const vector<Person*>& GetMembers() const { return m_pool.GetPool(); }

        /// Number of members of the household.
        size_t GetSize() const { return m_pool.GetSize(); }

        //Next 3 functions could be made private while declaring PopulationGenerator as a friend class...
        // second thought, after fixing PopulationGenerator I wonder if we need these at all...
        // if we do, they'll need to be slightly reworked...
        void GetSchoolAttendants(vector<Person*>&) const;

        void GetPossibleStudents(vector<Person*>&) const;

        void GetPossibleWorkers(vector<Person*>&) const;

private:
        static unsigned int m_id_generator; ///< Id generator.
        size_t m_id;            ///< A unique ID of the household.
        City* m_city;           ///< The City in which the household is located
        ContactPool m_pool;     ///< A ContactPool with the people belonging to this household.

};

} // namespace stride
