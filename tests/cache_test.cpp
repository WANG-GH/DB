//
// Created by yeye on 2021/4/15.
//

#include "cache.cpp"
#include<string>
#include<cstring>
#include "gtest/gtest.h"
TEST(CacheTest, TestHandleTable){

    HandleTable ht;
    for(int i = 0 ; i < 100 ; i++)
    {
        LRUHandle* e = new LRUHandle();
        e->value = nullptr;
        e->key_data = "key_data"+std::to_string(i);
        e->hash = Hash(e->key_data.c_str(), e->key_data.size(),1);
        e->deleter = nullptr;
        e->charge = 1;
        e->refs = 1;
        e->in_cache = false;
        ht.Insert(e);
    }

    Slice key = "key_data88";
    uint32_t hash = Hash(key.data(), key.size(), 1);
    LRUHandle* e = ht.Lookup(key,hash);
    ASSERT_TRUE(strcmp(e->key().data(), key.data()) == 0);
    key = "key_data888888";
    hash = Hash(key.data(), key.size(), 1);
    e = ht.Lookup(key, hash);
    ASSERT_TRUE(e == nullptr);
};

uint32_t Hhash(Slice str){
    return Hash(str.data(), str.size(),1);
}

TEST(CacheTest, TestLRUCache1){
    LRUCache* lc= new LRUCache();
    lc->SetCapacity(10);
    LRUHandle** handles = new LRUHandle*[100];
    for(int i = 0 ; i < 100 ; i++)
    {
        Slice s = "Key_data" + std::to_string(i);
        handles[i] =  lc->Insert(s, Hhash(s), nullptr, 1,
                                 [] (const Slice&, void* value)->void{
                                        return;
                                    });
    }
    Slice s = "Key_data" + std::to_string(5);
    LRUHandle* h =  lc->Lookup(s, Hhash(s));
    ASSERT_FALSE(strcmp(h->key().data(), s.data()));
    lc->Release(h);
    s = "Key_data666";
    h = lc->Lookup(s, Hhash(s));
    ASSERT_TRUE(h == nullptr);

    for(int i = 0; i < 100 ; i++)
        lc->Release(handles[i]);

    //when delete LRUCache, must promise the in_use is empty
    //otherwise it will touch the assert in LRUCache assert
    delete lc;
}

TEST(CacheTest, TestCache)
{
    Cache* c= new Cache(40);
    LRUHandle** handles = new LRUHandle*[100];
    for(int i = 0 ; i < 100 ; i++)
    {
        Slice s = "Key_data" + std::to_string(i);
        handles[i] =  c->Insert(s, nullptr, 1,
                                 [] (const Slice&, void* value)->void{
                                     return;
                                 });
    }
    Slice s = "Key_data" + std::to_string(5);
    LRUHandle* h =  c->Lookup(s);
    ASSERT_FALSE(strcmp(h->key().data(), s.data()));
    c->Release(h);
    s = "Key_data666";
    h = c->Lookup(s);
    ASSERT_TRUE(h == nullptr);

    for(int i = 0; i < 100 ; i++)
        c->Release(handles[i]);

    delete[] handles;
    //when delete LRUCache, must promise the in_use is empty
    //otherwise it will touch the assert in LRUCache assert
    delete c;
}