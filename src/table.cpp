//
// Created by yeye on 2021/4/17.
//

#include <table.h>

struct Table::Rep{
    RandomAccessFile* file;
    uint64_t file_size_;
    Block* index_block;
    BlockHandle meta_index_handle;
};


Status Table::Open(const Options &options, RandomAccessFile *file, uint64_t file_size, Table **table) {
    *table = nullptr;
    if (file_size < Footer::kEncodedLength) {
        return Status::Corruption("file is too short to be an sstable");
    }
    char footer_space[Footer::kEncodedLength];
    Slice footer_input;
    Status s = file->Read(file_size - Footer::kEncodedLength, Footer::kEncodedLength,
                          &footer_input, footer_space);
    if (!s.ok()) return s;

    Footer footer;
    s = footer.DecodeFrom(&footer_input);
    if (!s.ok()) return s;

    // Read the index block
    Slice index_block_contents;
    s = ReadBlock(file, options, footer.index_handle(), &index_block_contents);
    if (s.ok()) {
        // We've successfully read the footer and the index block: we're
        // ready to serve requests.
        Block* index_block = new Block(index_block_contents);
        Rep* rep = new Table::Rep;
        rep->file = file;
        rep->index_block = index_block;
        *table = new Table(rep);
        rep->meta_index_handle = footer.metaindex_handle();
    }
    return s;
}

Status Table::InternalGet(const Slice &key, void *arg, void (*handle_result)(void *, const Slice &, const Slice &)) {
    Status s;
    Iterator* iiter = rep_->index_block->NewIterator();
    iiter->Seek(key);
    if (iiter->Valid()) {
        Slice handle_value = iiter->value();
        Iterator* block_iter = BlockReader(this, options, iiter->value());
        block_iter->Seek(key);
        if (block_iter->Valid()) {
            (*handle_result)(arg, block_iter->key(), block_iter->value());
        }
        s = block_iter->status();
        delete block_iter;
    }

    delete iiter;
    return s;
}

Status Table::ReadBlock(RandomAccessFile *file, const Options &options, const BlockHandle &handle, Slice *result) {
    size_t n = static_cast<size_t>(handle.size_);
    char* buf = new char[n];
    Slice contents;
    Status s = file->Read(handle.offset_, n , &contents, buf);
    if (!s.ok()) {
        delete[] buf;
        return s;
    }
    *result = contents;
}

