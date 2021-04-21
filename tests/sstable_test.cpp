//
// Created by Linfp on 2021/4/20.
//
#include "gtest/gtest.h"
#include "table.h"
#include "table_builder.h"
#include "env.h"
TEST(SSTABLE_TEST,TEST_1){
    Env* e = new PosixEnv();
    WritableFile* file;
    e->NewWritableFile("hello.db",&file);
    Options options;
    TableBuilder table_builder(options,file);
    for(int i=0;i<10;i++){
        char * key = new char[100];
        char * value = new char[100];
        memcpy(key,std::string("key"+std::to_string(i)).data(),std::string("key"+std::to_string(i)).size());
        memcpy(value,std::string("value"+std::to_string(i)).data(),std::string("value"+std::to_string(i)).size());
        Slice k(key,std::string("key"+std::to_string(i)).size());
        Slice v(value,std::string("value"+std::to_string(i)).size());
        InternalKey *item = new InternalKey(i, kTypeValue,k,v);
        table_builder.Add(item);
    }
    table_builder.Finish();
    RandomAccessFile* rfile;
    uint64_t file_size;
    //TODO：env需要一个GetFileSize
    struct stat statbuf;
    stat("hello.db",&statbuf);
    e->NewRandomAccessFile("hello.db",&rfile);
    Table* table ;
    Table::Open(options,rfile,statbuf.st_size,&table);
    Iterator* iter = table->NewIterator(options);
    Slice s("key6");
    iter->Seek(s);
    if (iter->Valid()){
        ASSERT_EQ(iter->value(),"value"+std::to_string(6));
    }

}