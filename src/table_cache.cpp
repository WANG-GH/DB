//
// Created by yeye on 2021/4/17.
//

#include "table_cache.h"

Status TableCache::Get(uint64_t file_number, uint64_t file_size, const Slice &k, void *arg,
                       void (*handle_result)(void *, const Slice &, const Slice &)) {
    LRUHandle* handle = nullptr;
    //Status s = FindTable(file_number, file_size,&handle);
//    if(s.ok()){
//
//    }
}
