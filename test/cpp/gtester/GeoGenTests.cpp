//
// Created by robbe on 15/03/18.
//

#include <gtest/gtest.h>

int trivial(){
    return 0;
}

TEST(trivialTest, something){
 EXPECT_EQ(0, trivial());
}
