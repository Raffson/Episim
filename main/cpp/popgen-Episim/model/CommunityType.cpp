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
 *  Copyright 2017, Kuylen E, Willem L, Broeckhove J
 *  Copyright 2018, Raphael A
 */

/**
 * @file
 * Implementation of CommunityType.
 */

#include "CommunityType.h"


namespace stride {
namespace CommunityType {

using namespace std;
using boost::to_upper;

bool IsType(const string& s)
{
        static map<string, Id> ids{
            make_pair("COLLEGE", Id::College),
            make_pair("SCHOOL", Id::School),
            make_pair("WORK", Id::Work),
            make_pair("PRIMARY", Id::Primary),
            make_pair("SECONDARY", Id::Secondary),
        };
        string t{s};
        to_upper(t);
        return (ids.count(t) == 1);
}

string ToString(Id c)
{
        static map<Id, string> names{
            make_pair(Id::College, "college"),
            make_pair(Id::School, "school"),
            make_pair(Id::Work, "work"),
            make_pair(Id::Primary, "primary"),
            make_pair(Id::Secondary, "secondary"),
        };
        return (names.count(c) == 1) ? names[c] : "null";
}

Id ToType(const string& s)
{
        static map<string, Id> ids{
            make_pair("COLLEGE", Id::College),
            make_pair("SCHOOL", Id::School),
            make_pair("WORK", Id::Work),
            make_pair("PRIMARY", Id::Primary),
            make_pair("SECONDARY", Id::Secondary),
        };
        string t{s};
        to_upper(t);
        return (ids.count(t) == 1) ? ids[t] : throw runtime_error("CommunityType::ToString> not available:" + t);
}

Sizes ToSizes(Id id)
{
        static map<Id, Sizes> sizes{
            make_pair(Id::School, Sizes::SCHOOLS),
            make_pair(Id::College, Sizes::COLLEGES),
            make_pair(Id::Work, Sizes::WORKPLACES),
            make_pair(Id::Primary, Sizes::COMMUNITIES),
            make_pair(Id::Secondary, Sizes::COMMUNITIES),
        };
        return sizes[id];
}

ContactPoolType::Id ToContactPoolType(Id id)
{
        static map<Id, ContactPoolType::Id> ids{
            make_pair(Id::College, ContactPoolType::Id::School),
            make_pair(Id::School, ContactPoolType::Id::School),
            make_pair(Id::Work, ContactPoolType::Id::Work),
            make_pair(Id::Primary, ContactPoolType::Id::PrimaryCommunity),
            make_pair(Id::Secondary, ContactPoolType::Id::SecondaryCommunity),
        };
        return ids[id];
}

} // namespace CommunityType
} // namespace stride
