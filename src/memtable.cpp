//
// Created by Linfp on 2021/4/9.
//

#include "memtable.h"
#include "coding.h"
#include "status.h"

void MemTable::Add(SequenceNumber seq, ValueType type, const Slice &key, const Slice &value) {
    //seq和type共占16字节，seq 占56字节，type 占8字节
    table_.Insert(InternalKey(seq,type,key,value));
    memoryUsage_ += 16+key.size() + value.size();
    // Format of an entry is concatenation of:
    //  key_size     : int32 of internal_key.size()
    //  key bytes    : char[internal_key.size()]
    //  value_size   : int32 of value.size()
    //  value bytes  : char[value.size()]
    
    // char* buf =(char*)malloc(encoded_len);
    // buf = EncodeFixed32(buf,key_size);
    // buf = Copy(buf,key.data(),key_size);
    // buf = EncodeFixed64(buf,(seq << 8) | type);
    // buf = EncodeFixed32(buf,val_size);
    // buf = Copy(buf,value.data(),val_size);
    // assert(buf + val_size == buf + encoded_len);
    // table_->insert(buf);
}
bool MemTable::Get(const Slice  key, std::string* value, Status* s) {
    //LookupKey实际也是一个Internal_key
    InternalKey lookup_key = InternalKey(UINT64_MAX,kTypeValue,key,"");
    //实际就是跳表里的Iterator
    Table::Iterator iter(&table_);
    iter.Seek(lookup_key);
    if (iter.Valid()) {
        const InternalKey entry = iter.key();
        value->assign(entry.user_value.data(),entry.user_value.size());
        return true;
    }
    return false;
}