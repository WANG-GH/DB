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
        InternalKey internalKey(k,i,kTypeValue);

        builder.Add(internalKey.Encode(),v);
    }
    Slice content = builder.Finish();

    Block block(content);
    Slice s("key6");
    InternalKey internalKey(s,6,kTypeValue);
    BlockIterator* iter = new BlockIterator(&block);
    iter->Seek(internalKey.Encode());
    if (iter->Valid()){
            ASSERT_EQ(iter->value(),Slice("value6"));
    }

    for (int i = 0; i <10 ; i++) {
        Slice s("key"+std::to_string(i));
        InternalKey internalKey(s,i ,kTypeValue);
        iter->Seek(internalKey.Encode());
        if (iter->Valid()){
            ASSERT_EQ(iter->value(),Slice("value"+std::to_string(i)));
        }

    }
}