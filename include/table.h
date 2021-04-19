//
// Created by yeye on 2021/4/17.
//

#ifndef KVENGINE_TABLE_H
#define KVENGINE_TABLE_H
#include "env.h"
#include "options.h"
//#include "iterator.h"
#include "block.h"
class Table{
    static Status Open(const Options& options, RandomAccessFile* file,
                       uint64_t file_size, Table** table) ;

    Table(const Table&) = delete;
    Table& operator=(const Table&) = delete;
private:
    struct Rep;
    Table(Rep* rep): rep_(rep){};
    Status InternalGet(const Slice& key, void* arg,
                       void (*handle_result)(void* arg, const Slice& k, const Slice& v));
    Rep* const rep_;
    //static Iterator* BlockReader(void* arg, const Slice& index_value)
};

#endif //KVENGINE_TABLE_H
