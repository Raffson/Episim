/*
 *  This is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  The software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with the software. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2017, 2018 Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Implementation for the Simulator class.
 */

#include "Sim.h"

#include "behaviour/information_policies/LocalDiscussion.h"
#include "behaviour/information_policies/NoLocalInformation.h"
#include "calendar/Calendar.h"
#include "calendar/DaysOffStandard.h"
#include "contact/ContactHandler.h"
#include "contact/Infector.h"
#include "pool/ContactPoolType.h"
#include "pop/Population.h"

#include <trng/uniform01_dist.hpp>
#include <omp.h>

namespace stride {

using namespace std;
using namespace trng;
using namespace util;
using namespace ContactLogMode;

Sim::Sim()
    : m_config_pt(), m_contact_log_mode(Id::None), m_contact_profiles(), m_num_threads(1U), m_track_index_case(false),
      m_transmission_profile(), m_local_info_policy(), m_calendar(), m_population(nullptr), m_rn_manager()
{
        m_infectors = std::move(InfectorMap{
            // clang-format off
                {make_tuple(Id::Susceptibles, true, "NoLocalInformation"),
                        &Infector<Id::Susceptibles, true, NoLocalInformation>::Exec},
                {make_tuple(Id::All, true, "NoLocalInformation"),
                        &Infector<Id::All, true, NoLocalInformation>::Exec},
                {make_tuple(Id::Transmissions, true, "NoLocalInformation"),
                        &Infector<Id::Transmissions, true, NoLocalInformation>::Exec},
                {make_tuple(Id::None, true, "NoLocalInformation"),
                        &Infector<Id::None, true, NoLocalInformation>::Exec},
                {make_tuple(Id::Susceptibles, false, "NoLocalInformation"),
                        &Infector<Id::Susceptibles, false, NoLocalInformation>::Exec},
                {make_tuple(Id::All, false, "NoLocalInformation"),
                        &Infector<Id::All, false, NoLocalInformation>::Exec},
                {make_tuple(Id::Transmissions, false, "NoLocalInformation"),
                        &Infector<Id::Transmissions, false, NoLocalInformation>::Exec},
                {make_tuple(Id::None, false, "NoLocalInformation"),
                        &Infector<Id::None, false, NoLocalInformation>::Exec},
                {make_tuple(Id::Susceptibles, true, "LocalDiscussion"),
                        &Infector<Id::Susceptibles, true, LocalDiscussion>::Exec},
                {make_tuple(Id::All, true, "LocalDiscussion"),
                        &Infector<Id::All, true, LocalDiscussion>::Exec},
                {make_tuple(Id::Transmissions, true, "LocalDiscussion"),
                        &Infector<Id::Transmissions, true, LocalDiscussion>::Exec},
                {make_tuple(Id::None, true, "LocalDiscussion"),
                        &Infector<Id::None, true, LocalDiscussion>::Exec},
                {make_tuple(Id::Susceptibles, false, "LocalDiscussion"),
                        &Infector<Id::Susceptibles, false, LocalDiscussion>::Exec},
                {make_tuple(Id::All, false, "LocalDiscussion"),
                        &Infector<Id::All, false, LocalDiscussion>::Exec},
                {make_tuple(Id::Transmissions, false, "LocalDiscussion"),
                        &Infector<Id::Transmissions, false, LocalDiscussion>::Exec},
                {make_tuple(Id::None, false, "LocalDiscussion"),
                        &Infector<Id::None, false, LocalDiscussion>::Exec}
        }); // clang-format on
}

void Sim::TimeStep()
{
        std::shared_ptr<DaysOffInterface> daysOff{nullptr};

        // Logic where you compute (on the basis of input/config for initial day or on the basis of
        // number of sick persons, duration of epidemic etc) what kind of DaysOff scheme you apply.
        // If we want to make this independent of contacpools, then the daysOff object has to be
        // passed into the Update function.
        daysOff                = std::make_shared<DaysOffStandard>(m_calendar);
        const bool isWorkOff   = daysOff->IsWorkOff();
        const bool isSchoolOff = daysOff->IsSchoolOff();

        // Update individual's health status & presence in contactpools.
        for (auto& p : *m_population) {
                p.Update(isWorkOff, isSchoolOff);
        }

        // Update contact/transmission in contact pools & advance calendar.
        UpdatePools();
        m_calendar->AdvanceDay();
}

void Sim::UpdatePools()
{
        // Contact handlers, each bound to a generator bound to a different random engine stream.
        vector<ContactHandler> handlers;
        for (size_t i = 0; i < m_num_threads; i++) {
                // RN generators with double in [0.0, 1.0) each bound to a different stream.
                auto gen = m_rn_manager.GetGenerator(trng::uniform01_dist<double>(), i);
                handlers.emplace_back(ContactHandler(gen));
        }

        const auto simDay        = m_calendar->GetSimulationDay();
        auto&      poolSys       = m_population->GetContactPoolSys();
        auto       contactLogger = m_population->GetContactLogger();

        // Loop over the various types of contact pool systems (household, school, work, etc
        // Infector updates individuals for contacts & transmission within a pool.
#pragma omp parallel num_threads(m_num_threads)
        {
                const auto thread_num = static_cast<unsigned int>(omp_get_thread_num());
                for (auto typ : ContactPoolType::IdList) {
#pragma omp for schedule(runtime)
                        for (size_t i = 0; i < poolSys[typ].size(); i++) { // NOLINT
                                const auto s = make_tuple(m_contact_log_mode, m_track_index_case, m_local_info_policy);
                                (*m_infectors.at(s))(poolSys[typ][i], m_contact_profiles[typ], m_transmission_profile,
                                                     handlers[thread_num], simDay, contactLogger);
                        }
                }
        }
}

} // namespace stride
