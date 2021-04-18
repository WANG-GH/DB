//
// Created by yeye on 2021/4/13.
//

#ifndef KVENGINE_TABLE_CACHE_H
#define KVENGINE_TABLE_CACHE_H

#include "status.h"
#include "cache.h"
#include "options.h"
#include "table.h"
class TableCache{
public:
    Status Get(uint64_t file_number, uint64_t file_size,
               const Slice& k, void* arg,
               void (*handle)(void*, const Slice&, const Slice&));

private:

    Status FindTable(uint64_t file_number, uint64_t file_size, LRUHandle** handle);
    const std::string dbname_;
    const Options& options_;
    Cache* cache_;
};

#endif //KVENGINE_TABLE_CACHE_H
