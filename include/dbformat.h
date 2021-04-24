//
// Created by yeye on 2021/4/13.
//

#ifndef KVENGINE_DBFORMAT_H
#define KVENGINE_DBFORMAT_H
#include <cstdint>
#include "slice.h"
#include "comparator.h"
#include "coding.h"

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
struct ParsedInternalKey {
    Slice user_key;
    SequenceNumber sequence{};
    ValueType type;

    ParsedInternalKey() = default;  // Intentionally left uninitialized (for speed)
    ParsedInternalKey(const Slice& u, const SequenceNumber& seq, ValueType t)
            : user_key(u), sequence(seq), type(t) {}

};
bool ParseInternalKey(const Slice& internal_key, ParsedInternalKey* result);

// Returns the user key portion of an internal key.
inline Slice ExtractUserKey(const Slice& internal_key) {
    assert(internal_key.size() >= 8);
    return Slice(internal_key.data(), internal_key.size() - 8);
}

// A comparator for internal keys that uses a specified comparator for
// the user key portion and breaks ties by decreasing sequence number.
//class InternalKeyComparator : public Comparator {
//private:
//    const Comparator* user_comparator_;
//
//public:
//    explicit InternalKeyComparator(const Comparator* c) : user_comparator_(c) {}
//    const char* Name() const override;
//    int Compare(const Slice& a, const Slice& b) const override;
//    void FindShortestSeparator(std::string* start,
//                               const Slice& limit) const override;
//    void FindShortSuccessor(std::string* key) const override;
//
//    const Comparator* user_comparator() const { return user_comparator_; }
//
//    int Compare(const InternalKey& a, const InternalKey& b) const;
//};


static const SequenceNumber kMaxSequenceNumber = ((0x1ull << 56) - 1);
static const ValueType kValueTypeForSeek = kTypeValue;
static uint64_t PackSequenceAndType(uint64_t seq, ValueType t) {
    assert(seq <= kMaxSequenceNumber);
    assert(t <= kValueTypeForSeek);
    return (seq << 8) | t;
}
void AppendInternalKey(std::string* result, const ParsedInternalKey& key);
class InternalKey {
private:
    std::string rep_;

public:
    InternalKey() = default;  // Leave rep_ as empty to indicate it is invalid
    InternalKey(const Slice& user_key, SequenceNumber s, ValueType t) {
        AppendInternalKey(&rep_, ParsedInternalKey(user_key, s, t));
    }

    bool DecodeFrom(const Slice& s) {
        rep_.assign(s.data(), s.size());
        return !rep_.empty();
    }

    Slice Encode() const {
        assert(!rep_.empty());
        return rep_;
    }

    Slice user_key() const { return ExtractUserKey(rep_); }

    void SetFrom(const ParsedInternalKey& p) {
        rep_.clear();
        AppendInternalKey(&rep_, p);
    }
    void Clear() { rep_.clear(); }

};

//class InternalKeyComparator : public Comparator {
//public:
//    const char* Name() const override{
//        return NULL;
//    }
//    int Compare(const Slice& a, const Slice& b) const override{
//        return 0;
//    }
//    void FindShortestSeparator(std::string* start,
//                               const Slice& limit) const override{
//    }
//    void FindShortSuccessor(std::string* key) const override{
//    }
//    //以上方法是因为从虚基类 Comparator 继承需要实现
//    int Compare(const InternalKey& a, const InternalKey& b) const{
//        int r =a.user_key.compare(b.user_key);
//        if (r!=0){
//            return r;
//        }
//        return a.seq > b.seq ? -1:1;
//    }
//};

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
