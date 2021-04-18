//
// Created by yeye on 2021/4/14.
//

#include "cache.h"
#include <string>
#include "slice.h"
//struct LRUHandle{
//    void* value;    //table and file
//    void (*deleter)(const Slice& key, void* value); //for polymorphism
//    LRUHandle* next_hash;
//    LRUHandle* next;
//    LRUHandle* prev;
//    uint32_t refs;
//    uint32_t hash;
//    size_t charge;
//    bool in_cache;
//    std::string key_data;
//
//    Slice key() const{
//        return Slice(key_data);
//    }
//};
//

//hash map, for the next_hash in LRUHandle
//class HandleTable{
//public:
//    HandleTable(): length_(2), elems_(0), list_(nullptr){Resize();}
//    ~HandleTable() {delete[] list_;}
//    LRUHandle* Insert(LRUHandle* h);
//    LRUHandle* Remove(const Slice& key, uint32_t hash);
//    LRUHandle* Lookup(const Slice& key, uint32_t hash);
//private:
//    uint32_t elems_;
//    uint32_t length_;
//    LRUHandle** list_;
//
//    void Resize();
//    LRUHandle** FindPointer(const Slice& key, uint32_t hash);
//};


void HandleTable::Resize() {
    uint32_t new_length = 4;
    while (new_length < elems_) {
        new_length *= 2;
    }
    LRUHandle** new_list = new LRUHandle* [new_length];
    memset(new_list, 0, new_length * sizeof(LRUHandle*));
    uint32_t count = 0;
    for(int i = 0 ; i < length_ ; i++)
    {
        LRUHandle* h = list_[i];
        while(h != nullptr)
        {
            LRUHandle* next = h->next_hash;
            uint32_t hash = h->hash;

            LRUHandle** ptr =  &new_list[hash & (new_length - 1)];
            h->next_hash = *ptr;
            *ptr = h;
            h = next;
            count++;
        }
    }

    list_ = new_list;
    elems_ = count;
    length_ = new_length;
}

LRUHandle *HandleTable::Lookup(const Slice &key, uint32_t hash) {
    return *FindPointer(key, hash);
}

LRUHandle **HandleTable::FindPointer(const Slice &key, uint32_t hash) {
    LRUHandle** ptr = &list_[hash& (length_ - 1)];
    while(*ptr != nullptr && ((*ptr)->hash != hash || (*ptr)->key() != key)){
        ptr = &(*ptr)->next_hash;
    }
    return ptr;
}

LRUHandle *HandleTable::Insert(LRUHandle *h) {
    LRUHandle** ptr = FindPointer(h->key(),h->hash);
    LRUHandle* old = *ptr;

    h->next_hash = (old == nullptr ? nullptr : old->next_hash);
    *ptr = h;

    if(old == nullptr){
        elems_++;
        if(elems_ > length_)
            Resize();
    }

    return old;
}

LRUHandle *HandleTable::Remove(const Slice &key, uint32_t hash) {
    LRUHandle** ptr = FindPointer(key, hash);
    LRUHandle* result = *ptr;
    if (result != nullptr) {
        *ptr = result->next_hash;
        --elems_;
    }
    return result;
}

//inset new, unref old judge lru
LRUHandle*
LRUCache::Insert(const Slice &key, uint32_t hash, void *value,
                 size_t charge,
                 void (*deleter)(const Slice &,
                                 void *)) {
    LRUHandle* e = new LRUHandle();
    e->value = value;
    e->key_data = key.ToString();
    e->hash = hash;
    e->deleter = deleter;
    e->charge = charge;
    e->refs = 1;    //for the return handle
    e->in_cache = false;

    if(capacity_ > 0)
    {
        e->refs++;
        e->in_cache = true;
        LRU_Append(&in_use_, e);
        FinishErase(table_.Insert(e));  //delete the old node in the table;
    }
    else{
        e->next = nullptr;
    }

    //not understand. why old = lru next? lru_next and lru_??
    while (usage_ > capacity_ && lru_.next != &lru_) {
        LRUHandle* old = lru_.next;
        //assert(old->refs == 1);
        bool erased = FinishErase(table_.Remove(old->key(), old->hash));
        if (!erased) {  // to avoid unused variable when compiled NDEBUG
            assert(erased);
        }
    }
    return e;
}

void LRUCache::LRU_Append(LRUHandle *list, LRUHandle *e) {
    // Make "e" newest entry by inserting just before *list
    e->next = list;
    e->prev = list->prev;
    e->prev->next = e;
    e->next->prev = e;
}

bool LRUCache::FinishErase(LRUHandle *e) {
    if(e != nullptr)
    {
        //assert(e->in_cache)
        usage_ -= e->charge;
        LRU_Remove(e);
        e->in_cache = false;
        Unref(e);
    }
    return e != nullptr;
}

void LRUCache::LRU_Remove(LRUHandle *e) {
    e->prev->next = e->next;
    e->next->prev = e->prev;
}

//safely handle the LRUHandle. delete or move to the head
void LRUCache::Unref(LRUHandle* e) {
    //assert(e->refs > 0);
    e->refs--;
    if (e->refs == 0) {  // Deallocate.
        assert(!e->in_cache);
        (*e->deleter)(e->key(), e->value);
        free(e);
    } else if (e->in_cache && e->refs == 1) {
        // No longer in use; move to lru_ list.
        // not understand is the oldest in the head of list?
        LRU_Remove(e);
        LRU_Append(&lru_, e);
    }
}

LRUHandle *LRUCache::Lookup(const Slice &key, uint32_t hash) {
    LRUHandle* e = table_.Lookup(key, hash);
    if(e != nullptr)
        Ref(e);
    return e;
}

void LRUCache::Ref(LRUHandle *e) {
    if(e->refs == 1 && e->in_cache)
    {
        //if in lru_list, move to in_use_ list
        LRU_Remove(e);
        LRU_Append(&in_use_, e);
    }
    e->refs++;
}

void LRUCache::Release(LRUHandle *handle) {
    Unref(handle);
}

void LRUCache::Erase(const Slice &key, uint32_t hash) {
    FinishErase(table_.Remove(key,hash));
}

void LRUCache::Prune() {
    while (lru_.next != &lru_) {
        LRUHandle* e = lru_.next;
        //assert(e->refs == 1);
        bool erased = FinishErase(table_.Remove(e->key(), e->hash));
        if (!erased) {  // to avoid unused variable when compiled NDEBUG
            assert(erased);
        }
    }
}

LRUCache::LRUCache() {
    //circular linked lists
    lru_.next = &lru_;
    lru_.prev = &lru_;
    in_use_.next = &in_use_;
    in_use_.prev = &in_use_;
}

LRUCache::~LRUCache() {
    assert(in_use_.next == &in_use_);  // Error if caller has an unreleased handle
    for (LRUHandle* e = lru_.next; e != &lru_;) {
        LRUHandle* next = e->next;
        assert(e->in_cache);
        e->in_cache = false;
        assert(e->refs == 1);  // Invariant of lru_ list.
        Unref(e);
        e = next;
    }
}

size_t LRUCache::TotalCharge() const {
    return usage_;
}

uint32_t HDecodeFixed32(const char* ptr) {
    const uint8_t* const buffer = reinterpret_cast<const uint8_t*>(ptr);

    // Recent clang and gcc optimize this to a single mov / ldr instruction.
    return (static_cast<uint32_t>(buffer[0])) |
           (static_cast<uint32_t>(buffer[1]) << 8) |
           (static_cast<uint32_t>(buffer[2]) << 16) |
           (static_cast<uint32_t>(buffer[3]) << 24);
}
uint32_t Hash(const char *data, size_t n, uint32_t seed) {
    // Similar to murmur hash
    const uint32_t m = 0xc6a4a793;
    const uint32_t r = 24;
    const char* limit = data + n;
    uint32_t h = seed ^ (n * m);

    // Pick up four bytes at a time
    while (data + 4 <= limit) {
        uint32_t w = HDecodeFixed32(data);
        data += 4;
        h += w;
        h *= m;
        h ^= (h >> 16);
    }

    // Pick up remaining bytes
    switch (limit - data) {
        case 3:
            h += static_cast<uint8_t>(data[2]) << 16;
            FALLTHROUGH_INTENDED;
        case 2:
            h += static_cast<uint8_t>(data[1]) << 8;
            FALLTHROUGH_INTENDED;
        case 1:
            h += static_cast<uint8_t>(data[0]);
            h *= m;
            h ^= (h >> r);
            break;
    }
    return h;
}

Slice LRUHandle::key() const {
    return Slice(key_data);
}

LRUHandle *Cache::Insert(const Slice &key, void *value, size_t charge, void (*deleter)(const Slice &, void *)) {
    uint32_t hash = HashSlice(key);
    return shared_[Shard(hash)].Insert(key, hash, value, charge, deleter);
}

LRUHandle *Cache::Lookup(const Slice &key) {
    uint32_t hash = HashSlice(key);
    return shared_[Shard(hash)].Lookup(key, hash);
}

void Cache::Release(LRUHandle *handle) {
    shared_[Shard(handle->hash)].Release(handle);
}

void *Cache::Value(LRUHandle *handle) {
    return handle->value;
}

void Cache::Erase(const Slice& key) {
    uint32_t hash = HashSlice(key);
    shared_[Shard(hash)].Erase(key, hash);
}

size_t Cache::TotalCharge() const {
    int total = 0;
    for(int i = 0 ; i < 4 ; i ++){
        total += shared_[i].TotalCharge();
    }
    return total;
}

uint64_t Cache::NewId() {
    return ++last_id_;
}

void Cache::Prune() {
    for(int i = 0 ; i < 4 ; i ++){
        shared_[i].Prune();
    }
}

uint32_t Cache::HashSlice(const Slice &s) {
    return Hash(s.data(), s.size(), 0);
}

uint32_t Cache::Shard(uint32_t hash) {
    return hash >> (32 - 16);
}

Cache::Cache(size_t capacity) {
    const size_t per_shard = (capacity + (4 - 1)) / 4;
    for (int s = 0; s < 4; s++) {
        shared_[s].SetCapacity(per_shard);
    }
}
