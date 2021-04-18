//
// Created by yeye on 2021/4/17.
//

#include <table.h>

struct Table::Rep{
    RandomAccessFile* file;
    uint64_t file_size_;

};

Status Table::Open(const Options &options, RandomAccessFile *file, uint64_t file_size, Table **table) {

}


