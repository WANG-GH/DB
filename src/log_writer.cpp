
//
// Created by yeye on 2021/4/19.
//

#include "log_writer.h"

Status Writer::AddRecord(const Slice &slice) {
    const char* data = slice.data();
    int left = slice.size();
    RecordType type;
    Status s;
    bool begin = true;
    do{
        int leftover = kBlockSize - block_offset;
        if(leftover < kHeaderSize)
        {
            file_->Append(Slice("\x00\x00\x00\x00\x00\x00", leftover));
            block_offset = 0;
        }

        int avail = kBlockSize - block_offset - kHeaderSize;
        int e_avail = (left < avail) ? left : avail;
        bool end = (left == e_avail);

        if(begin && end){
            type = kFullType;
        }else if(begin){
            type = kFirstType;
        }else if(end){
            type = kLastType;
        }else{
            type = kMiddleType;
        }

        data += e_avail;
        left -= e_avail;

        s = EmitPhysicalRecord(type, data, e_avail);
        begin = false;
    }while(left > 0);
}
