//
// Created by robbe on 15/03/18.
//

#include <gtest/gtest.h>

#include "popgen-Episim/GeoGen/GeoGrid.h"

TEST(GeoGenTest, happy_day_constructor){

    EXPECT_NO_THROW(geogen::GeoGrid("config/geogen_default.xml"));

    geogen::GeoGrid grid("config/geogen_default.xml");

}
