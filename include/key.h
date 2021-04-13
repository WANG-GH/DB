//
// Created by Linfp on 2021/4/13.
//

#ifndef KVENGINE_KEY_H
#define KVENGINE_KEY_H

#include <cstdint>
#include "slice.h"
#include "comparator.h"

enum ValueType { kTypeDeletion = 0x0, kTypeValue = 0x1 };
typedef uint64_t SequenceNumber;

class InternalKey{

public:
    InternalKey(){}
    //TODO:字节对齐
    InternalKey(SequenceNumber seq_,ValueType type_,
                const Slice key_,const Slice value_):
            seq(seq_),type(type_),user_key(key_),user_value(value_){}
    void EncodeTo(std::string *dst){
        char  buf[4];
        memcpy(buf,&seq,8);
        dst->append(buf,8);
        memcpy(buf,&type,1);
        dst->append(buf,1);
        int32_t key_size = user_key.size();
        memcpy(buf,&key_size,4);
        dst->append(buf,4);
        dst->append(user_key.data(),user_key.size());
        int32_t value_size = user_value.size();
        memcpy(buf,&value_size,4);
        dst->append(buf,4);
        dst->append(user_value.data(),user_value.size());
    }
    //return the record length
    int64_t DecodeFrom(const char *buf){
        int offset=0;
        memcpy(&seq,buf+offset,8);
        offset+=8;
        memcpy(&type,buf+offset,1);
        offset+=1;
        int32_t key_size ;
        memcpy(&key_size,buf+offset,4);
        offset+=4;
        char *key_buf = new char [key_size];
        memcpy(key_buf,buf+offset,key_size);
        offset+=key_size;
        user_key=Slice(key_buf,key_size);
        int32_t value_size ;
        memcpy(&value_size,buf+offset,4);
        offset+=4;
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

#endif //KVENGINE_KEY_H
