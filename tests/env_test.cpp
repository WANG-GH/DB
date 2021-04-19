//
// Created by yeye on 2021/4/13.
//

#include "gtest/gtest.h"
#include "env.h"
#include <iostream>

TEST(envTest, Test1){
    Env e;
    std::string str("testDB");
    Status s = e.CreateDir(str);
    ASSERT_TRUE(s.ok());
    str = "Makefile";
    //s = e.FileExists(str);
    //ASSERT_TRUE(s.ok());
}

TEST(envTest, TestFileStatus){
    FileState fs;
    for(int i = 0; i < 2000; i++)
    {
        std::string val = "----Data" + std::to_string(i);
        fs.Append(Slice(val));
    }
    ASSERT_TRUE(fs.DebugBlockSize() == 3);
    Slice result;
    char* scr = new char[11];
    fs.Read(0,9, &result, scr);
    ASSERT_TRUE(strcmp(scr, "----Data0") == 0);
    fs.Read(990, 11, &result, scr);
    ASSERT_TRUE(strcmp(scr, "----Data100") == 0);
}

TEST(envTest, TestWritableFile){
    FileState* fs = new FileState;
    WritableFile wf(fs);
    //TODO: finish Flush() and test.
}
