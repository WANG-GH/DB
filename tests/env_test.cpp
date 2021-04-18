//
// Created by yeye on 2021/4/13.
//

#include "gtest/gtest.h"
#include "env.h"

//class envTest : public testing::Test{
//protected:
//    virtual void SetUp(){
//
//    }
//};

TEST(envTest, Test1){
    Env e;
    std::string str("testDB");
    Status s = e.CreateDir(str);
    ASSERT_TRUE(s.ok());
    str = "Makefile";
    s = e.FileExists(str);
    ASSERT_TRUE(s.ok());
}
