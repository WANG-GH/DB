//
// Created by Linfp on 2021/4/13.
//

#ifndef KVENGINE_BLOCK_BUILDER_H
#define KVENGINE_BLOCK_BUILDER_H

#include "dbformat.h"
#include "slice.h"
#include <string>
#include <vector>
struct Slot{
    uint64_t offset;
    uint64_t length;
    Slot(int64_t off,int64_t len){
        offset = off;
        length = len;
    }
};
struct BlockTrailer{
    uint64_t counter;
};


class  BlockBuilder  {
public:
    BlockBuilder(){
        buffer_.clear();
        slots.clear();
    }
    void Add(const Slice& key,const Slice& value);
    Slice Finish();
    inline void Reset(){
        slots.clear();
        buffer_.clear();
    }
    inline int CurrentSizeEstimate(){
        return buffer_.size();
    }
    inline bool IsEmpty(){
        return buffer_.size()==0;
    }
private:
    std::string buffer_;
    std::vector<Slot> slots;
};



#endif //KVENGINE_BLOCK_BUILDER_H
