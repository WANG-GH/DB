//
// Created by Linfp on 2021/4/9.
//

#include "gtest/gtest.h"
#include "memtable.h"

TEST(MEMTABLE_TEST,TEST_1){
    const InternalKeyComparator internal_comparator_;
    MemTable * memtable = new MemTable(internal_comparator_);

    for (int i = 0; i < 100; ++i) {
        char * key = new char[100];
        char * value = new char[100];
        memcpy(key,std::string("key"+std::to_string(i)).data(),std::string("key"+std::to_string(i)).size());
        memcpy(value,std::string("value"+std::to_string(i)).data(),std::string("value"+std::to_string(i)).size());
        Slice k(key,std::string("key"+std::to_string(i)).size());
        Slice v(value,std::string("value"+std::to_string(i)).size());
        memtable->Add(i,kTypeValue,k,v);
    }
    for (int i = 200; i >= 100; i--) {
        char * key = new char[100];
        char * value = new char[100];
        memcpy(key,std::string("key"+std::to_string(i)).data(),std::string("key"+std::to_string(i)).size());
        memcpy(value,std::string("value"+std::to_string(i)).data(),std::string("value"+std::to_string(i)).size());
        Slice k(key,std::string("key"+std::to_string(i)).size());
        Slice v(value,std::string("value"+std::to_string(i)).size());
        memtable->Add(i,kTypeValue,k,v);
    }
    for (int i = 0; i <= 200; ++i) {
        std::string value;
        Status s ;
        memtable->Get(Slice("key"+std::to_string(i)),&value,&s);
        ASSERT_EQ(value,"value"+std::to_string(i));
    }
}
#if 0
TEST(SKIPLIST_TEST,TEST_GET_SIZE){
    SkipList * memtable;
    memtable = new SkipList(6,"");
    int size = 0;
    for (int i = 0; i < 100; ++i) {
        std::string key("key"+std::to_string(i));
        std::string val("val"+ std::to_string(i));
        memtable->insert(key,val);
        size+=key.size()+val.size();
    }
    ASSERT_EQ(size,memtable->getSize());
}
TEST(SKIPLIST_TEST,TEST_INSERT_FIND){
    SkipList * memtable;
    memtable = new SkipList(6,"");
    for (int i = 0; i < 100; ++i) {
        std::string key("key"+std::to_string(i));
        std::string val("val"+ std::to_string(i));
        memtable->insert(key,val);
    }
    for (int i = 0; i < 100; ++i) {
        ASSERT_EQ(memtable->find("key"+std::to_string(i))->value,std::string("val"+std::to_string(i)));
    }

}
#endif