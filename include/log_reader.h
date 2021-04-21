//
// Created by yeye on 2021/4/20.
//

#ifndef KVENGINE_LOG_READER_H
#define KVENGINE_LOG_READER_H
#include "env.h"
#include "slice.h"
#include "dbformat.h"
class Reader{
public:
    Reader(SequentialFile* file): file_(file){}
    ~Reader() = default;
    bool ReadRecord(Slice *result, std::string *scratch);

private:
    int ReadPhysicsRecord(Slice *result);
    SequentialFile* file_;
    Slice buffer_;
    char back_store_[kBlockSize];
    size_t block_pos;
    bool eof_;
};

#endif //KVENGINE_LOG_READER_H
