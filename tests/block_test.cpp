//
// Created by Linfp on 2021/4/13.
//

#include "block.h"
#include "block_builder.h"
#include "iterator.h"
#include "gtest/gtest.h"
TEST(BLOCK_TEST,TEST_1){
    BlockBuilder builder;
    for(int i=0;i<10;i++){
        char * key = new char[100];
        char * value = new char[100];
        memcpy(key,std::string("key"+std::to_string(i)).data(),std::string("key"+std::to_string(i)).size());
        memcpy(value,std::string("value"+std::to_string(i)).data(),std::string("value"+std::to_string(i)).size());
        Slice k(key,std::string("key"+std::to_string(i)).size());
        Slice v(value,std::string("value"+std::to_string(i)).size());
        InternalKey *item = new InternalKey(i, kTypeValue,k,v);
        builder.Add(item);
    }
    Slice content = builder.Finish();

    Block block(content);
    Slice s("key6");
    BlockIterator* iter = new BlockIterator(&block);
    iter->Seek(s);
    if (iter->Valid()){
        if (iter->key() == s){
            ASSERT_EQ(iter->value(),Slice("value6"));
        }
    }

    for (int i = 0; i <10 ; i++) {
        Slice s("key"+std::to_string(i));
        iter->Seek(s);
        if (iter->Valid()){
            ASSERT_EQ(iter->value(),"value"+std::to_string(i));
        }

    }
}