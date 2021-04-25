//
// Created by Linfp on 2021/4/24.
//

#include "coding.h"
#include "block_builder.h"

Slice BlockBuilder::Finish() {
    BlockTrailer blockTrailer;
    blockTrailer.counter = slots.size();
    for (auto slot:slots){
        PutFixed64(&buffer_,slot.offset);
        PutFixed64(&buffer_,slot.length);
    }
    PutFixed64(&buffer_,blockTrailer.counter);
    return Slice(buffer_);
}

void BlockBuilder::Add(const Slice &key, const Slice &value) {
    size_t record_size = key.size() + value.size() + sizeof(uint32_t) *2;
    slots.push_back(Slot(buffer_.size(),record_size));
    PutFixed32(&buffer_,key.size());
    buffer_.append(key.data(),key.size());
    PutFixed32(&buffer_,value.size());
    buffer_.append(value.data(),value.size());
}
