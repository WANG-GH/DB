//
// Created by yeye on 2021/4/20.
//

#include "log_writer.h"
#include "log_reader.h"
#include "gtest/gtest.h"
TEST(LogTest, TestWriter){
    Env* e = new PosixEnv;
    WritableFile* wf;
    bool s = e->NewWritableFile("testLog", &wf);
    ASSERT_TRUE(s);
    Writer writer(wf);
    s = writer.AddRecord(Slice("this is "+std::to_string(0)+"."));
    ASSERT_TRUE(s);
    for(int i = 0 ; i < 10000 ; i++) {
        writer.AddRecord(Slice("this is "+std::to_string(i)+"."));
    }
}

TEST(LogTest, TestReader){
    Env* e = new PosixEnv;
    SequentialFile* sf;
    e->NewSequentialFile("testLog", &sf);
    Reader reader(sf);
    Slice s;
    std::string scratch;
    reader.ReadRecord(&s, &scratch);
    ASSERT_TRUE(strcmp(s.data(), "this is 0.") == 0);
    for(int i = 0 ; i < 9999 ; i++) //3331
    {
        reader.ReadRecord(&s, &scratch);
        std::string str = "this is "+std::to_string(i)+".";
        bool k = strcmp(s.data(), str.c_str()) == 0;
        ASSERT_TRUE(strcmp(s.data(), str.c_str()) == 0);
    }
    //reader.ReadRecord(&s, &scratch);
}