//
// Created by yeye on 2021/4/19.
//

#ifndef KVENGINE_LOG_WRITER_H
#define KVENGINE_LOG_WRITER_H
#include "env.h"
//#include "status.h"
#include "dbformat.h"
class Writer{
public:
    explicit Writer(WritableFile* dest): block_offset(0), file_(dest){}
    bool AddRecord(const Slice& data);
    bool test();
private:
    WritableFile* file_;
    int block_offset;   //the offset in current block
    bool EmitPhysicalRecord(RecordType t, const char* src, size_t length) ;
};

#endif //KVENGINE_LOG_WRITER_H
