//
// Created by Linfp on 2021/4/9.
//

#include "gtest/gtest.h"
#include "memtable.h"

class BytewiseComparatorImpl : public Comparator {
public:
    BytewiseComparatorImpl() = default;

    const char *Name() const override { return "leveldb.BytewiseComparator"; }

    int Compare(const Slice &a, const Slice &b) const override {
        return a.compare(b);
    }

    void FindShortestSeparator(std::string *start,
                               const Slice &limit) const override {
        // Find length of common prefix
        size_t min_length = std::min(start->size(), limit.size());
        size_t diff_index = 0;
        while ((diff_index < min_length) &&
               ((*start)[diff_index] == limit[diff_index])) {
            diff_index++;
        }

        if (diff_index >= min_length) {
            // Do not shorten if one string is a prefix of the other
        } else {
            uint8_t diff_byte = static_cast<uint8_t>((*start)[diff_index]);
            if (diff_byte < static_cast<uint8_t>(0xff) &&
                diff_byte + 1 < static_cast<uint8_t>(limit[diff_index])) {
                (*start)[diff_index]++;
                start->resize(diff_index + 1);
                assert(Compare(*start, limit) < 0);
            }
        }
    }

    void FindShortSuccessor(std::string *key) const override {
        // Find first character that can be incremented
        size_t n = key->size();
        for (size_t i = 0; i < n; i++) {
            const uint8_t byte = (*key)[i];
            if (byte != static_cast<uint8_t>(0xff)) {
                (*key)[i] = byte + 1;
                key->resize(i + 1);
                return;
            }
        }
        // *key is a run of 0xffs.  Leave it alone.
    }
};

TEST(MEMTABLE_TEST, TEST_ADD_GET) {
    Comparator *usr_comparator = new BytewiseComparatorImpl();
    const InternalKeyComparator internal_comparator_(usr_comparator);
    MemTable *memtable = new MemTable(internal_comparator_);

    for (int i = 0; i < 100; ++i) {
        char *key = new char[100];
        char *value = new char[100];
        memcpy(key, std::string("key" + std::to_string(i)).data(), std::string("key" + std::to_string(i)).size());
        memcpy(value, std::string("value" + std::to_string(i)).data(), std::string("value" + std::to_string(i)).size());
        Slice k(key, std::string("key" + std::to_string(i)).size());
        Slice v(value, std::string("value" + std::to_string(i)).size());
        memtable->Add(i, kTypeValue, k, v);
    }
    for (int i = 200; i >= 100; i--) {
        char *key = new char[100];
        char *value = new char[100];
        memcpy(key, std::string("key" + std::to_string(i)).data(), std::string("key" + std::to_string(i)).size());
        memcpy(value, std::string("value" + std::to_string(i)).data(), std::string("value" + std::to_string(i)).size());
        Slice k(key, std::string("key" + std::to_string(i)).size());
        Slice v(value, std::string("value" + std::to_string(i)).size());
        memtable->Add(i, kTypeValue, k, v);
    }
    for (int i = 0; i <= 200; ++i) {
        std::string value;
        Status s;
        LookupKey lookupKey(Slice("key" + std::to_string(i)),i);
        memtable->Get(lookupKey, &value, &s);
        ASSERT_EQ(value, "value" + std::to_string(i));
    }
}
TEST(MEMTABLE_TEST, TEST_MEM_ITERATOR) {
    Comparator *usr_comparator = new BytewiseComparatorImpl();
    const InternalKeyComparator internal_comparator_(usr_comparator);
    MemTable *memtable = new MemTable(internal_comparator_);

    for (int i = 0; i < 100; ++i) {
        char *key = new char[100];
        char *value = new char[100];
        memcpy(key, std::string("key" + std::to_string(i)).data(), std::string("key" + std::to_string(i)).size());
        memcpy(value, std::string("value" + std::to_string(i)).data(), std::string("value" + std::to_string(i)).size());
        Slice k(key, std::string("key" + std::to_string(i)).size());
        Slice v(value, std::string("value" + std::to_string(i)).size());
        memtable->Add(i, kTypeValue, k, v);
    }
    for (int i = 200; i >= 100; i--) {
        char *key = new char[100];
        char *value = new char[100];
        memcpy(key, std::string("key" + std::to_string(i)).data(), std::string("key" + std::to_string(i)).size());
        memcpy(value, std::string("value" + std::to_string(i)).data(), std::string("value" + std::to_string(i)).size());
        Slice k(key, std::string("key" + std::to_string(i)).size());
        Slice v(value, std::string("value" + std::to_string(i)).size());
        memtable->Add(i, kTypeValue, k, v);
    }

    int looks[10]={12,41,5,6,7,8,3,5,21,99};
    Iterator* iter = memtable->NewIterator();
    iter->SeekToFirst();
    for (int i = 0; i < 10; ++i) {
        char *key = new char[100];
        char *value = new char[100];
        memcpy(key, std::string("key" + std::to_string(looks[i])).data(), std::string("key" + std::to_string(looks[i])).size());
        memcpy(value, std::string("value" + std::to_string(looks[i])).data(), std::string("value" + std::to_string(looks[i])).size());
        Slice v(value, std::string("value" + std::to_string(looks[i])).size());
        Slice k(key, std::string("key" + std::to_string(looks[i])).size());
        InternalKey internalKey(k,looks[i],kTypeValue);
        iter->Seek(internalKey.Encode());
        ASSERT_EQ(iter->value(),v);
    }
}



