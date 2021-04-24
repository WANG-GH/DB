//
// Created by yeye on 2021/4/13.
//

#include "gtest/gtest.h"
#include "env.h"
#include <iostream>

TEST(envTest, Test1){
//    Env e;
//    std::string str("testDB");
//    Status s = e.CreateDir(str);
//    ASSERT_TRUE(s.ok());
//    str = "Makefile";
//    //s = e.FileExists(str);
//    //ASSERT_TRUE(s.ok());
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
    Env* e = new PosixEnv;
    WritableFile* wf ;
    e->NewWritableFile(std::string("TestFile1"), &wf);
    wf->Append(Slice("testAppend"));
    for(int i = 0 ; i < 1000; i++)
    {
        wf->Append(Slice("testData"+std::to_string(i)));
    }
    wf->Flush();
    wf->Close();

    RandomAccessFile* raf;
    e->NewRandomAccessFile(std::string("TestFile1"), &raf);
    Slice s;
    char scratch[10];
    raf->Read(0, 10, &s, scratch);
    ASSERT_TRUE(strcmp(scratch, "testAppend") == 0);
    char scratch2[9];
    raf->Read(37, 9, &s, scratch2);
    ASSERT_TRUE(strncmp(scratch2, "testData3", 9) == 0);
    ASSERT_TRUE(strncmp("testData3", s.data(), 9) == 0);
}
