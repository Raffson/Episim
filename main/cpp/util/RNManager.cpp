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
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Implementation of RNManager.
 */

#include "RNManager.h"
#include "util/RunConfigManager.h"

#include <sstream>
#include <fstream>

#include <boost/property_tree/xml_parser.hpp>

using namespace std;
using namespace stride::util;
using namespace stride::RNEngineType;

namespace stride {
namespace util {

// This function is used as a quick utility routine
// Push the combined state of the engine streams to a stringstream.
template <typename T>
void para_info(stringstream& ss, const vector<T>& engines)
{
        for (const auto& e : engines) {
                ss << e;
        }
}

RNManager::RNManager(const Info& info)
    : m_seed(), m_stream_count(), m_type_id(), m_lcg64(), m_lcg64_shift(), m_mrg2(), m_mrg3(), m_yarn2(), m_yarn3()
{
        Initialize(info);
}

RNManager::Info RNManager::GetInfo() const
{
        Info         info;
        stringstream ss;

        switch (m_type_id) {
        case Id::lcg64: para_info(ss, m_lcg64); break;
        case Id::lcg64_shift: para_info(ss, m_lcg64_shift); break;
        case Id::mrg2: para_info(ss, m_mrg2); break;
        case Id::mrg3: para_info(ss, m_mrg3); break;
        case Id::yarn2: para_info(ss, m_yarn2); break;
        case Id::yarn3: para_info(ss, m_yarn3); break;
        }

        info.m_seed         = m_seed;
        info.m_state        = ss.str();
        info.m_stream_count = m_stream_count;
        info.m_type         = ToString(m_type_id);
        return info;
}

void RNManager::Initialize(const Info& info)
{
        m_seed         = info.m_seed;
        m_stream_count = info.m_stream_count;
        m_type_id      = ToType(info.m_type);

        switch (m_type_id) {
        case Id::lcg64: para_seed(m_lcg64, info); break;
        case Id::lcg64_shift: para_seed(m_lcg64_shift, info); break;
        case Id::mrg2: para_seed(m_mrg2, info); break;
        case Id::mrg3: para_seed(m_mrg3, info); break;
        case Id::yarn2: para_seed(m_yarn2, info); break;
        case Id::yarn3: para_seed(m_yarn3, info); break;
        }
}

template <typename T>
void RNManager::para_seed(vector<T>& engines, const Info& info)
{
        auto state = info.m_state;
        engines.resize(m_stream_count);
        if (state.empty()) {
                for (size_t i = 0; i < m_stream_count; ++i) {
                        engines[i].seed(m_seed);
                        engines[i].split(m_stream_count, i);
                }
        } else {
                stringstream ss(state);
                for (size_t i = 0; i < m_stream_count; ++i) {
                        ss >> engines[i];
                        engines[i].split(m_stream_count, i);
                }
        }
}

void RNManager::StateFromFile(const string& fname)
{
        boost::property_tree::ptree pt;
        boost::property_tree::read_xml(fname, pt);
        unsigned long seed       = pt.get<unsigned long>("rng_state.seed", m_seed);
        string        type       = pt.get("rng_state.type", ToString(m_type_id));
        string        state      = pt.get("rng_state.state", GetInfo().m_state);
        unsigned int  numThreads = pt.get<unsigned int>("rng_state.stream_count", m_stream_count);
        Initialize(util::RNManager::Info{type, seed, state, numThreads});
}

void RNManager::StateToFile(const string &fname) const
{
        boost::property_tree::ptree pt;
        pt.put("rng_state.seed", m_seed);
        pt.put("rng_state.state", GetInfo().m_state);
        pt.put("rng_state.stream_count", m_stream_count);
        pt.put("rng_state.type", ToString(m_type_id));
        std::ofstream file;
        file.open(fname.c_str(), std::ofstream::out);
        file << util::RunConfigManager::ToString(pt) << endl;
        file.close();
}

} // namespace util
} // namespace stride
