//
// Created by Linfp on 2021/4/13.
//

#ifndef KVENGINE_BLOCK_BUILDER_H
#define KVENGINE_BLOCK_BUILDER_H

#include "key.h"
#include "slice.h"
#include <string>

class  BlockBuilder  {
public:
    BlockBuilder(){
        buffer_.clear();
        counter_ = 0;
    }
    void Add(InternalKey* item){
        counter_++;
        item->EncodeTo(&buffer_);
    }
    Slice Finish(){
        char  buf[4];
        memcpy(buf,&counter_,4);
        buffer_.append(buf,4);
        return Slice(buffer_);
    }
    void Reset(){
        counter_=0;
        buffer_.clear();
    }
    int CurrentSizeEstimate(){
        return buffer_.size();
    }
    bool IsEmpty(){
        return buffer_.size()==0;
    }
private:
    std::string buffer_;
    int32_t counter_;
};


#endif //KVENGINE_BLOCK_BUILDER_H
