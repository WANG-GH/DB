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

class InternalKeyComparator : public Comparator {
public:
    explicit InternalKeyComparator(const Comparator* c) : user_comparator_(c) {}
    const char* Name() const override{
        return "leveldb.InternalKeyComparator";
    }
    int Compare(const Slice& akey, const Slice& bkey) const override{
        // Order by:
        //    increasing user key (according to user-supplied comparator)
        //    decreasing sequence number
        //    decreasing type (though sequence# should be enough to disambiguate)
        int r = user_comparator_->Compare(ExtractUserKey(akey), ExtractUserKey(bkey));
        if (r == 0) {
            const uint64_t anum = DecodeFixed64(akey.data() + akey.size() - 8);
            const uint64_t bnum = DecodeFixed64(bkey.data() + bkey.size() - 8);
            if (anum > bnum) {
                r = -1;
            } else if (anum < bnum) {
                r = +1;
            }
        }
        return r;
    }
    void FindShortestSeparator(std::string* start,
                               const Slice& limit) const override{
    }
    void FindShortSuccessor(std::string* key) const override{
    }
    const Comparator* user_comparator() const { return user_comparator_; }
    //以上方法是因为从虚基类 Comparator 继承需要实现
    int Compare(const InternalKey& a, const InternalKey& b) const{
        return Compare(a.Encode(),b.Encode());
    }
private:
    const Comparator* user_comparator_;
};
class LookupKey {
public:
    // Initialize *this for looking up user_key at a snapshot with
    // the specified sequence number.
    LookupKey(const Slice& user_key, SequenceNumber sequence){
        size_t usize = user_key.size();
        size_t needed = usize + 13;  // A conservative estimate
        char* dst;
        if (needed <= sizeof(space_)) {
            dst = space_;
        } else {
            dst = new char[needed];
        }
        start_ = dst;
        dst = EncodeFixed32(dst, usize + 8);         // usize + 8 = internal_key_size
        kstart_ = dst;
        std::memcpy(dst, user_key.data(), usize);
        dst += usize;
        EncodeFixed64(dst, PackSequenceAndType(sequence, kValueTypeForSeek));
        dst += 8;
        end_ = dst;
    }

    LookupKey(const LookupKey&) = delete;
    LookupKey& operator=(const LookupKey&) = delete;

    ~LookupKey();

    // Return a key suitable for lookup in a MemTable.
    Slice memtable_key() const { return Slice(start_, end_ - start_); }

    // Return an internal key (suitable for passing to an internal iterator)
    Slice internal_key() const { return Slice(kstart_, end_ - kstart_); }

    // Return the user key
    Slice user_key() const { return Slice(kstart_, end_ - kstart_ - 8); }

private:
    // We construct a char array of the form:
    //    klength  varint32               <-- start_
    //    userkey  char[klength]          <-- kstart_
    //    tag      uint64
    //                                    <-- end_
    // The array is a suitable MemTable key.
    // The suffix starting with "userkey" can be used as an InternalKey.
    const char* start_;
    const char* kstart_;
    const char* end_;
    char space_[200];  // Avoid allocation for short keys

};
inline LookupKey::~LookupKey() {
    if (start_ != space_) delete[] start_;
}

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
