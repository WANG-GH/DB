//
// Created by yeye on 2021/4/13.
//

#ifndef KVENGINE_DBFORMAT_H
#define KVENGINE_DBFORMAT_H
#include <cstdint>
#include "slice.h"
#include "comparator.h"

//this file include dbformat and log_writer format

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
    void EncodeTo(std::string *dst){
        char  buf[8];
        memset(buf,0,sizeof(buf));
        memcpy(buf,&seq,8);
        dst->append(buf,8);
        memset(buf,0,sizeof(buf));
        memcpy(buf,&type,8);
        dst->append(buf,8);
        int64_t key_size = user_key.size();
        memcpy(buf,&key_size,8);
        dst->append(buf,8);
        dst->append(user_key.data(),user_key.size());
        int64_t value_size = user_value.size();
        memcpy(buf,&value_size,8);
        dst->append(buf,8);
        dst->append(user_value.data(),user_value.size());
    }
    //return the record length
    int64_t DecodeFrom(const char *buf){
        int offset=0;
        memcpy(&seq,buf+offset,8);
        offset+=8;
        memcpy(&type,buf+offset,8);
        offset+=8;
        int64_t key_size ;
        memcpy(&key_size,buf+offset,8);
        offset+=8;
        char *key_buf = new char [key_size];
        memcpy(key_buf,buf+offset,key_size);
        offset+=key_size;
        user_key=Slice(key_buf,key_size);
        int64_t value_size ;
        memcpy(&value_size,buf+offset,8);
        offset+=8;
        char *value_buf= new char [value_size];
        memcpy(value_buf,buf+offset,value_size);
        offset+=value_size;
        user_value=Slice(value_buf,value_size);
        return offset;
    }
    //all are public
public:
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

enum RecordType {
    // Zero is reserved for preallocated files
    kZeroType = 0,

    kFullType = 1,

    // For fragments
    kFirstType = 2,
    kMiddleType = 3,
    kLastType = 4
};
static const int kMaxRecordType = kLastType;

static const int kBlockSize = 32768;

// Header is checksum (4 bytes), length (2 bytes), type (1 byte).
static const int kHeaderSize = 4 + 2 + 1;

#endif //KVENGINE_DBFORMAT_H
