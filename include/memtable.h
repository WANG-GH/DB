//
// Created by Linfp on 2021/4/9.
//

#ifndef KVENGINE_MEMTABLE_H
#define KVENGINE_MEMTABLE_H
#include "skiplist.h"
#include "slice.h"
#include "comparator.h"
#include "status.h"
#include "dbformat.h"

// A helper class useful for DBImpl::Get()
#if 0
class LookupKey {
public:
    // Initialize *this for looking up user_key at a snapshot with
    // the specified sequence number.
    LookupKey(const Slice& user_key, SequenceNumber sequence);

    LookupKey(const LookupKey&) = delete;
    LookupKey& operator=(const LookupKey&) = delete;


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
    ~LookupKey(){
         if (start_ != space_) delete[] start_;
    }
};
#endif

class MemTable {
public:
    MemTable(const InternalKeyComparator& comparator)
            : comparator_(comparator), table_(comparator_, &arena_) {}
    MemTable(const MemTable&) = delete;
    MemTable& operator=(const MemTable&) = delete;

    // Returns an estimate of the number of bytes of data in use by this
    // data structure. It is safe to call when MemTable is being modified.
    inline size_t ApproximateMemoryUsage(){
        return memoryUsage_;
    }

    // Return an iterator that yields the contents of the memtable.
    //
    // The caller must ensure that the underlying MemTable remains live
    // while the returned iterator is live.  The keys returned by this
    // iterator are internal keys encoded by AppendInternalKey in the
    // db/format.{h,cc} module.

    // Add an entry into memtable that maps key to value at the
    // specified sequence number and with the specified type.
    // Typically value will be empty if type==kTypeDeletion.
    void Add(SequenceNumber seq,ValueType type, const Slice& key,
             const Slice& value);

    // If memtable contains a value for key, store it in *value and return true.
    // If memtable contains a deletion for key, store a NotFound() error
    // in *status and return true.
    // Else, return false.
    bool Get(const Slice  key, std::string* value, Status* s);

private:
    struct KeyComparator {
        const InternalKeyComparator comparator;
        explicit KeyComparator(const InternalKeyComparator& c) : comparator(c) {}
        int operator()(const InternalKey& a, const InternalKey& b) const{
            return comparator.Compare(a,b);
        }
    };
    typedef SkipList<InternalKey, KeyComparator> Table;
    ~MemTable();  // Private since only Unref() should be used to delete it
    KeyComparator comparator_;
    Table table_;
    uint64_t memoryUsage_;
    Arena arena_;
};


#endif //KVENGINE_MEMTABLE_H
