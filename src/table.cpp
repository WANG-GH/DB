//
// Created by yeye on 2021/4/17.
//

#include <table.h>

struct Table::Rep{
    RandomAccessFile* file;
    uint64_t file_size_;
    Block* index_block;
};

Status Table::Open(const Options &options, RandomAccessFile *file, uint64_t file_size, Table **table) {

}

Status Table::InternalGet(const Slice &key, void *arg, void (*handle_result)(void *, const Slice &, const Slice &)) {
//    Status s;
//    Iterator* iiter = rep_->index_block->NewIterator(rep_->options.comparator);
//    iiter->Seek(key);
//    if (iiter->Valid()) {
//        Slice handle_value = iiter->value();
//        Iterator* block_iter = BlockReader(this, options, iiter->value());
//        block_iter->Seek(key);
//        if (block_iter->Valid()) {
//            (*handle_result)(arg, block_iter->key(), block_iter->value());
//        }
//        s = block_iter->status();
//        delete block_iter;
//    }
//    if (s.ok()) {
//        s = iiter->status();
//    }
//    delete iiter;
//    return s;
}


