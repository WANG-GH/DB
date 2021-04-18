//
// Created by yeye on 2021/4/14.
//

#ifndef KVENGINE_CACHE_H
#define KVENGINE_CACHE_H
#include "slice.h"
#include <string>

class LRUHandle;
class LRUCache;
class HandleTable;
class Cache;

#ifndef FALLTHROUGH_INTENDED
#define FALLTHROUGH_INTENDED \
  do {                       \
  } while (0)
#endif

uint32_t Hash(const char* data, size_t n, uint32_t seed);

struct LRUHandle{
    void* value;    //table and file
    void (*deleter)(const Slice& key, void* value); //for polymorphism
    LRUHandle* next_hash;
    LRUHandle* next;
    LRUHandle* prev;
    uint32_t refs;
    uint32_t hash;
    size_t charge;
    bool in_cache;
    std::string key_data;

    Slice key() const;
};

class HandleTable{
public:
    HandleTable(): length_(0), elems_(0), list_(nullptr){Resize();}
    ~HandleTable() {delete[] list_;}
    LRUHandle* Insert(LRUHandle* h);
    LRUHandle* Remove(const Slice& key, uint32_t hash);
    LRUHandle* Lookup(const Slice& key, uint32_t hash);
private:
    uint32_t elems_;
    uint32_t length_;
    LRUHandle** list_;

    void Resize();
    LRUHandle** FindPointer(const Slice& key, uint32_t hash);
};

class LRUCache{
public:
    LRUCache();
    ~LRUCache();

    void SetCapacity(size_t capacity){capacity_ = capacity;};
    LRUHandle* Insert(const Slice& key, uint32_t hash, void* value,
                      size_t charge, void (*deleter)(const Slice& key, void* value));
    LRUHandle* Lookup(const Slice& key, uint32_t hash);
    void Release(LRUHandle* handle);
    void Erase(const Slice& key, uint32_t hash);
    void Prune();
    size_t TotalCharge() const;

private:
    size_t capacity_;
    size_t usage_;
    LRUHandle in_use_;  //the head node? where to initialize
    LRUHandle lru_; //not understand
    HandleTable table_;

    void Ref(LRUHandle* e);
    void Unref(LRUHandle* e);
    bool FinishErase(LRUHandle* e);
    void LRU_Append(LRUHandle* list, LRUHandle* e);
    void LRU_Remove(LRUHandle* e);
};

class Cache{
public:
    Cache(size_t capacity);
    Cache(const Cache&) = delete;
    Cache& operator=(const Cache) = delete;

    LRUHandle* Insert(const Slice& key, void* value, size_t charge,
                      void (*deleter)(const Slice& key, void* value));
    LRUHandle* Lookup(const Slice& key);
    void Release(LRUHandle* handle);
    void* Value(LRUHandle* handle);
    void Erase(const Slice& key);
    size_t TotalCharge() const;
    //for multiple client
    uint64_t NewId();
    void Prune();

private:
    LRUCache shared_[4];
    uint64_t last_id_;
    static inline uint32_t HashSlice(const Slice& s);
    static uint32_t Shard(uint32_t hash);
};
#endif //KVENGINE_CACHE_H
