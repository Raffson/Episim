//
// Created by beau on 3/5/18.
//

#ifndef STRIDE_COMMUNITY_H
#define STRIDE_COMMUNITY_H

#include <cstddef>
#include <string>

namespace geogen {

    /*
    enum class CommunityType
    {
        School,
        University,
        Workplace,
        Primary,
        Secondary,
        Null
    };
    */

    class Community {
    public:
        Community() = default;

        ///
        //Community(CommunityType community_type, std::size_t city_id, unsigned int size);
        Community(std::size_t city_id, unsigned int size);

        ///
        //Community(std::size_t id, CommunityType community_type, std::size_t city_id, unsigned int size);
        Community(std::size_t id, std::size_t city_id, unsigned int size);

        ///
        std::size_t GetCityID() { return m_city_id; }

        ///
        //CommunityType GetCommunityType() { return m_community_type; }
        std::string GetCommunityType() { return std::string(""); }

        ///
        std::size_t GetID() { return m_community_id; }

        ///
        unsigned int GetSize() { return m_size; }


    private:
        std::size_t m_community_id;   ///< A unique ID for the community
        //CommunityType m_community_type; ///< The type of community
        std::size_t m_city_id;        ///< The ID of the city in which the community is located
        unsigned int m_size;           ///< The maximum number of people that can part of this community.
    };

}//namespace geogen

#endif //STRIDE_COMMUNITY_H
