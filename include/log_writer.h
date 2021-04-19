//
// Created by yeye on 2021/4/19.
//

#ifndef KVENGINE_LOG_WRITER_H
#define KVENGINE_LOG_WRITER_H
#include "env.h"
#include "dbformat.h"
class Writer{
public:
    Status AddRecord(const Slice& data);

private:
    WritableFile* file_;
    int block_offset;   //the offset in current block
    Status EmitPhysicalRecord(RecordType t, const char* src, size_t length) ;
};

#endif //KVENGINE_LOG_WRITER_H
