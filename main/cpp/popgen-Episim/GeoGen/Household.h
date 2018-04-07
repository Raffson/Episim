//
// Created by beau on 3/5/18.
//

#ifndef STRIDE_HOUSEHOLD_H
#define STRIDE_HOUSEHOLD_H

#include <cstddef>
#include <vector>
#include "pop/Person.h"

using namespace std;


class Household
{
public:
        Household();

        /// Add a member to the household.
        void AddMember(const shared_ptr<stride::Person>& member);

        /// Id of city where housholds lives.
        size_t GetCityID() const { return m_city_id; }

        /// Household Id.
        size_t GetID() const { return m_id; }

        /// Id's of members of household.
        vector<shared_ptr<stride::Person>> GetMembers() const { return m_members; }

        /// Number of members of the household.
        size_t GetSize() const { return m_members.size(); }

        /// Set Id for city where household lives.
        void SetCityID(size_t city_id);

        //Next 3 functions could be made private while declaring PopulationGenerator as a friend class...
        void GetSchoolAttendants(vector<shared_ptr<stride::Person>>&) const;

        void GetPossibleStudents(vector<shared_ptr<stride::Person>>&) const;

        void GetPossibleWorkers(vector<shared_ptr<stride::Person>>&) const;

private: // Raphael@everyone replace size_t by unsigned int?
        /// Id generator.
        static size_t g_id_generator;

        /// A unique ID of the household.
        size_t m_id;

        /// A ContactPool with the people belonging to this household.
        std::shared_ptr<stride::ContactPool> m_members;

        ///< The ID of the city in which the household is located
        size_t m_city_id;
};

#endif // STRIDE_HOUSEHOLD_H
