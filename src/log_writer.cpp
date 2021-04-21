
//
// Created by yeye on 2021/4/19.
//

#include "log_writer.h"

bool Writer::AddRecord(const Slice &slice) {
    const char* data = slice.data();
    int left = slice.size();
    RecordType type;
    bool s;
    bool begin = true;
    do{
        int leftover = kBlockSize - block_offset;
        if(leftover < kHeaderSize)
        {
            file_->Append(Slice("\x00\x00\x00\x00\x00\x00\x00", leftover));
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
        s = EmitPhysicalRecord(type, data, e_avail);
        data += e_avail;
        left -= e_avail;

        begin = false;
    }while(left > 0 && s);
    return s;
}

bool Writer::EmitPhysicalRecord(RecordType t, const char *src, size_t length) {
    char buffer[kHeaderSize];
    memset(buffer, 0, 4);
    buffer[4] = static_cast<char>(length & 0xff);
    buffer[5] = static_cast<char>(length >> 8);
    buffer[6] = static_cast<char>(t);

    bool s;
    s = file_->Append(Slice(buffer, kHeaderSize));
    if(s) s = file_->Append(Slice(src, length));
    if(s) s = file_->Flush();
    block_offset += kHeaderSize + length;
    return s;
}

bool Writer::test() {
    return false;
}