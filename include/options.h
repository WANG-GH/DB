//
// Created by yeye on 2021/4/13.
//

#ifndef KVENGINE_OPTIONS_H
#define KVENGINE_OPTIONS_H
#include"comparator.h"
struct Options{
    const Comparator* comparator;

    // Amount of data to build up in memory (backed by an unsorted log
    // on disk) before converting to a sorted on-disk file.
    size_t write_buffer_size = 4 * 1024 * 1024;

    size_t block_size = 4 * 1024;

};
#endif //KVENGINE_OPTIONS_H
