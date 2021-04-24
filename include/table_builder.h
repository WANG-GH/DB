//
// Created by Linfp on 2021/4/18.
//

#ifndef KVENGINE_TABLE_BUILDER_H
#define KVENGINE_TABLE_BUILDER_H

#include <cstdint>
#include <fcntl.h>
#include <unistd.h>
#include "options.h"
#include "status.h"
#include "block_builder.h"
#include "block_handle.h"
#include "env.h"
#include "iterator.h"



class BlockBuilder;

class BlockHandle;

class WritableFile;

class TableBuilder {
public:

    TableBuilder(const Options &options,WritableFile* file );

    TableBuilder(const TableBuilder &) = delete;

    TableBuilder &operator=(const TableBuilder &) = delete;

    ~TableBuilder();

    void Add(const Slice& key, const Slice& value);

    void Flush();

    Status status() const{return Status();}

    void Finish();

private:


    void WriteBlock(BlockBuilder *block,BlockHandle *handle);
    void WriteRawBlock(const Slice &block_contents, BlockHandle *handle);

    struct Rep;
    Rep* rep_;


};
struct TableBuilder::Rep {
    Rep(const Options& opt, WritableFile* f)
            : options(opt),
              file(f),
              offset(0),
              num_entries(0),
              closed(false),
              pending_index_entry(false){}
    Options options;
    WritableFile* file;
    uint64_t offset;
    Status status;
    BlockBuilder data_block_builder;
    BlockBuilder index_block_builder;
    std::string last_key;
    int64_t num_entries;
    bool closed;  // Either Finish() or Abandon() has been called.

    // Invariant: r->pending_index_entry is true only if data_block is empty.
    bool pending_index_entry;
    BlockHandle pending_handle;  // Handle to add to index block

};

TableBuilder::TableBuilder(const Options &options, WritableFile *file)
        :rep_(new Rep(options,file)){
}

TableBuilder::~TableBuilder() {
//    assert(rep_->closed);  // Catch errors where caller forgot to call Finish()
    delete rep_;
}

void TableBuilder:: Add(const Slice& key, const Slice& value){
    Rep* r = rep_;
    assert(!r->closed);
    if (r->num_entries > 0) {
        assert(r->options.comparator->Compare(key, Slice(r->last_key)) > 0);
    }
    if (r->pending_index_entry) {
        assert(r->data_block_builder.IsEmpty());
        r->options.comparator->FindShortestSeparator(&r->last_key, key);
        std::string handle_encoding;
        r->pending_handle.EncodeTo(&handle_encoding);
        r->index_block_builder.Add(r->last_key, Slice(handle_encoding));
        r->pending_index_entry = false;
    }

    r->last_key.assign(key.data(), key.size());
    r->num_entries++;
    r->data_block_builder.Add(key,value);
    if(r->data_block_builder.CurrentSizeEstimate()> r->options.block_size){
        Flush();
    }
}

void TableBuilder::Finish() {
    Rep* r = rep_;
    //write remaining data block
    Flush();
    assert(!r->closed);
    r->closed = true;

    BlockHandle index_block_handle;
    //write indexblock
    if(rep_->pending_index_entry){
        r->options.comparator->FindShortSuccessor(&r->last_key);
        std::string handle_encoding;
        r->pending_handle.EncodeTo(&handle_encoding);
        r->index_block_builder.Add(r->last_key, Slice(handle_encoding));
        r->pending_index_entry = false;
    }
    WriteBlock(&r->index_block_builder, &index_block_handle);

    // Write footer
    Footer footer;
    footer.set_index_handle(index_block_handle);
    std::string footer_encoding;
    footer.EncodeTo(&footer_encoding);
    bool ret = r->file->Append(footer_encoding);
    if (ret)
    {
        r->offset+=footer_encoding.size();
    }
    r->file->Close();
}

void TableBuilder::Flush() {
    Rep* r = rep_;
    assert(!r->closed);
    if (r->data_block_builder.IsEmpty()) return;
    assert(!r->pending_index_entry);
    WriteBlock(&r->data_block_builder, &r->pending_handle);
    r->pending_index_entry = true;
    r->file->Flush();
}

void TableBuilder::WriteBlock(BlockBuilder *block_builder,BlockHandle* handle) {

    Rep* r = rep_;
    Slice raw = block_builder->Finish();
    WriteRawBlock(raw, handle);
    block_builder->Reset();
}
void TableBuilder::WriteRawBlock(const Slice& block_contents,BlockHandle* handle) {
    Rep* r = rep_;
    handle->set_offset(r->offset);
    handle->set_length(block_contents.size());
    bool ret = r->file->Append(block_contents);
    r->offset+= block_contents.size();
    assert(ret);
}

#endif //KVENGINE_TABLE_BUILDER_H
