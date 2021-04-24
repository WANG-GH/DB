//
// Created by Linfp on 2021/4/9.
//

#include "memtable.h"
#include "coding.h"
#include "status.h"

static Slice GetLengthPrefixedSlice(const char* data) {
    uint32_t len=DecodeFixed32(data);
    // +4 : we assume "p" is not corrupted
    return Slice(data+4, len);
}
void MemTable::Add(SequenceNumber seq, ValueType type, const Slice &key, const Slice &value) {
    // Format of an entry is concatenation of:
    //  key_size     : varint32 of internal_key.size()
    //  key bytes    : char[internal_key.size()]
    //  value_size   : varint32 of value.size()
    //  value bytes  : char[value.size()]
    InternalKey internal_key(key, seq, type);
    Slice encoded_internal_key = internal_key.Encode();
    uint32_t internal_key_size = internal_key.Encode().size();
    uint32_t val_size = value.size();
    const size_t encoded_len = sizeof(uint32_t) + encoded_internal_key.size() + sizeof(uint32_t) + val_size;
    char *buf = arena_.Allocate(encoded_len);
    char *p = EncodeFixed32(buf, internal_key_size);
    memcpy(p, internal_key.Encode().data(), internal_key_size);
    p += internal_key_size;
    p = EncodeFixed32(p, val_size);
    memcpy(p, value.data(), val_size);
    assert(p + val_size == buf + encoded_len);
    table_.Insert(buf);
}

bool MemTable::Get(const LookupKey &key, std::string *value, Status *s) {
    Slice memkey = key.memtable_key();
    Table::Iterator iter(&table_);
    iter.Seek(memkey.data());
    if (iter.Valid()) {
        // entry format is:
        //    klength  varint32
        //    userkey  char[klength]
        //    tag      uint64
        //    vlength  varint32
        //    value    char[vlength]
        // Check that it belongs to same user key.  We do not check the
        // sequence number since the Seek() call above should have skipped
        // all entries with overly large sequence numbers.
        const char *entry = iter.key();
        const char *key_ptr = entry + sizeof(uint32_t);
        uint32_t key_length = DecodeFixed32(entry);
        if (comparator_.comparator.user_comparator()->Compare(
                Slice(key_ptr, key_length - 8), key.user_key()) == 0) {
            // Correct user key
            const uint64_t tag = DecodeFixed64(key_ptr + key_length - 8);
            switch (static_cast<ValueType>(tag & 0xff)) {
                case kTypeValue: {
                    Slice v = GetLengthPrefixedSlice(key_ptr + key_length);
                    value->assign(v.data(), v.size());
                    return true;
                }
                case kTypeDeletion:
                    *s = Status::NotFound(Slice());
                    return true;
            }
        }
    }
    return false;
}

MemTable::~MemTable() {

}


int MemTable::KeyComparator::operator()(const char *aptr, const char *bptr) const {
    // Internal keys are encoded as length-prefixed strings.
    Slice a = GetLengthPrefixedSlice(aptr);
    Slice b = GetLengthPrefixedSlice(bptr);
    return comparator.Compare(a, b);
}
