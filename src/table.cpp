//
// Created by yeye on 2021/4/17.
//

#include <table.h>

struct Table::Rep{
    Options options;
    RandomAccessFile* file;
    Block* index_block;
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
    }
    return s;
}

Status Table::InternalGet(const Slice &key,Slice * value) {
    Status s;
    Iterator* iiter = rep_->index_block->NewIterator();
    iiter->Seek(key);
    if (iiter->Valid()) {
        Slice handle_value = iiter->value();
        Iterator* block_iter = GetDataIter(handle_value);
        block_iter->Seek(key);
        if (block_iter->Valid()) {
            *value = block_iter->value();
        }
        delete block_iter;
    }

    delete iiter;
    return s;
}

Status Table::ReadBlock(RandomAccessFile *file, const Options &options, const BlockHandle &handle, Slice *result) {
    size_t n = static_cast<size_t>(handle.size());
    char* buf = new char[n];
    Slice contents;
    Status s = file->Read(handle.offset(), n , &contents, buf);
    if (!s.ok()) {
        delete[] buf;
        return s;
    }
    *result = contents;
    return  Status();
}

Iterator *Table::NewIterator(const Options &options) {
    return new TableIterator(this,options);
}

Iterator *Table::GetDataIter(Slice &index_value) {
    Block* block = nullptr;
    BlockHandle handle;
    Slice input = index_value;
    Status s = handle.DecodeFrom(&input);
    Slice result;
    s =ReadBlock(rep_->file, rep_->options,handle,&result);
    if (s.ok()) {
        block = new Block(result);
    }
    Iterator * iter = nullptr;
    if (block!= nullptr){
        iter = block->NewIterator();
    }
    return iter;
}

TableIterator::TableIterator(Table *table, const Options &options)
        : options_(options),table_(table),data_iter_(nullptr){
    index_iter_ = table_->rep_->index_block->NewIterator();
}

