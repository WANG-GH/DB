//
// Created by Linfp on 2021/4/9.
//

#ifndef KVENGINE_MEMTABLE_H
#define KVENGINE_MEMTABLE_H
#include <utility>

#include "skiplist.h"
#include "slice.h"
#include "comparator.h"
#include "status.h"
#include "dbformat.h"
#include "iterator.h"


class MemTableIterator;

class MemTable {
public:
    MemTable(const InternalKeyComparator& comparator)
            : comparator_(comparator), table_(comparator_, &arena_) {}
    MemTable(const MemTable&) = delete;
    MemTable& operator=(const MemTable&) = delete;
    Iterator* NewIterator();
    // Returns an estimate of the number of bytes of data in use by this
    // data structure. It is safe to call when MemTable is being modified.
    inline     uint64_t ApproximateMemoryUsage(){
        return arena_.MemoryUsage();
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
    bool Get(const LookupKey & lookupKey, std::string* value, Status* s);
    ~MemTable();  // Private since only Unref() should be used to delete it
private:
    friend class MemTableIterator;
    struct KeyComparator {
        const InternalKeyComparator comparator;
        explicit KeyComparator(InternalKeyComparator  c) : comparator(std::move(c)) {}
        int operator()(const char* a, const char* b) const;
    };
    typedef SkipList<const char*, KeyComparator> Table;

    KeyComparator comparator_;
    Table table_;
    Arena arena_;
};


#endif //KVENGINE_MEMTABLE_H
