//
// Created by yeye on 2021/4/13.
//

#ifndef KVENGINE_DBFORMAT_H
#define KVENGINE_DBFORMAT_H
#include <cstdint>
#include "slice.h"
#include "comparator.h"
namespace config {
    static const int kNumLevels = 7;

// Level-0 compaction is started when we hit this many files.
    static const int kL0_CompactionTrigger = 4;

// Soft limit on number of level-0 files.  We slow down writes at this point.
    static const int kL0_SlowdownWritesTrigger = 8;

// Maximum number of level-0 files.  We stop writes at this point.
    static const int kL0_StopWritesTrigger = 12;

// Maximum level to which a new compacted memtable is pushed if it
// does not create overlap.  We try to push to level 2 to avoid the
// relatively expensive level 0=>1 compactions and to avoid some
// expensive manifest file operations.  We do not push all the way to
// the largest level since that can generate a lot of wasted disk
// space if the same key space is being repeatedly overwritten.
    static const int kMaxMemCompactLevel = 2;

// Approximate gap in bytes between samples of data read during iteration.
    static const int kReadBytesPeriod = 1048576;

}  // namespace config

typedef uint64_t SequenceNumber;
enum ValueType { kTypeDeletion = 0x0, kTypeValue = 0x1 };
class LookupKey{

};

class InternalKey{
public:
    InternalKey(){}
    InternalKey(SequenceNumber seq_,ValueType type_,
                const Slice key_,const Slice value_):
            seq(seq_),type(type_),user_key(key_),user_value(value_){}
    SequenceNumber seq;
    ValueType type;
    Slice user_key;
    Slice user_value;
};

class InternalKeyComparator : public Comparator {
public:
    const char* Name() const override{
        return NULL;
    }
    int Compare(const Slice& a, const Slice& b) const override{
        return 0;
    }
    void FindShortestSeparator(std::string* start,
                               const Slice& limit) const override{
    }
    void FindShortSuccessor(std::string* key) const override{
    }
    //以上方法是因为从虚基类 Comparator 继承需要实现
    int Compare(const InternalKey& a, const InternalKey& b) const{
        int r =a.user_key.compare(b.user_key);
        if (r!=0){
            return r;
        }
        return a.seq > b.seq ? -1:1;
    }
};
#endif //KVENGINE_DBFORMAT_H
