//
// Created by yeye on 2021/4/20.
//

#include "log_reader.h"

int Reader::ReadPhysicsRecord(Slice *result) {
    bool s;
    if(buffer_.size() < kHeaderSize)
    {
        buffer_.clear();
        file_->Read(kBlockSize, &buffer_, back_store_);
        if(buffer_.size() < kBlockSize)
            eof_ = true;
    }

    const char* header = buffer_.data();
    const uint32_t a = static_cast<uint32_t>(header[4]) & 0xff;
    const uint32_t b = static_cast<uint32_t>(header[5]) & 0xff;
    const int type = static_cast<int>(header[6]);
    const uint32_t length = a | (b << 8);

    buffer_.remove_prefix(kHeaderSize + length);
    *result = Slice(header + kHeaderSize, length);

    return type;
}

bool Reader::ReadRecord(Slice *result, std::string *scratch) {
    scratch->clear();
    Slice tem_result;
    while(true){
        int type = ReadPhysicsRecord(&tem_result);
        switch (type){
            case kFullType:
                *result = tem_result;
                return true;

            case kFirstType:
                scratch->append(tem_result.data(), tem_result.size());
                break;

            case kMiddleType:
                scratch->append(tem_result.data(), tem_result.size());
                break;

            case kLastType:
                scratch->append(tem_result.data(), tem_result.size());
                *result = Slice(*scratch);
                return true;
                break;
        }
    }
}
