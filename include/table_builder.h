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

const int  MAX_BLOCK_SIZE = 4 * 1024;

class BlockBuilder;

class BlockHandle;

class WritableFile;

class TableBuilder {
public:

    TableBuilder(const Options &options,WritableFile* file );

    TableBuilder(const TableBuilder &) = delete;

    TableBuilder &operator=(const TableBuilder &) = delete;

    ~TableBuilder();

    void Add(InternalKey* );

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
    //std::string last_key;
    int64_t num_entries;
    bool closed;  // Either Finish() or Abandon() has been called.

    bool pending_index_entry;
    IndexBlockHandle pending_handle;  // Handle to add to index block

};

TableBuilder::TableBuilder(const Options &options, WritableFile *file)
        :rep_(new Rep(options,file)){
}

TableBuilder::~TableBuilder() {
//    assert(rep_->closed);  // Catch errors where caller forgot to call Finish()
    delete rep_;
}

void TableBuilder::Add(InternalKey* item) {
    Rep* r = rep_;
    assert(!r->closed);

    if (r->num_entries > 0) {
        //assert(r->options.comparator->Compare(key, Slice(r->last_key)) > 0);
    }
    if (r->pending_index_entry) {
        assert(r->data_block_builder.IsEmpty());
        r->index_block_builder.Add(rep_->pending_handle.internalKey);
        r->pending_index_entry = false;
    }
    rep_->pending_handle.internalKey = item;

    r->num_entries++;
    r->data_block_builder.Add(item);
    if(rep_->data_block_builder.CurrentSizeEstimate()> MAX_BLOCK_SIZE){
        Flush();
    }
}

void TableBuilder::Finish() {
    Rep* r = rep_;
    //write data block
    Flush();
    assert(!r->closed);
    r->closed = true;

    //write indexblock
    if(rep_->pending_index_entry){
        rep_->index_block_builder.Add(rep_->pending_handle.internalKey);
        rep_->pending_index_entry = false;
    }
    Footer footer;
    BlockHandle handle ;
    WriteBlock(&rep_->index_block_builder,&handle);
    footer.set_index_handle(handle);
    //write footer
    std::string footer_encoding;
    footer.EncodeTo(&footer_encoding);
    rep_->file->Append(footer_encoding);
    rep_->file->Close();
    rep_->offset+=footer_encoding.size();
    return ;
}

void TableBuilder::Flush() {
    Rep* r = rep_;
    assert(!r->closed);

    if (r->data_block_builder.IsEmpty()) return;
    assert(!r->pending_index_entry);
    BlockHandle handle;
    WriteBlock(&r->data_block_builder, &handle);
    InternalKey* origin_key = r->pending_handle.internalKey;
    r->pending_handle.internalKey = new InternalKey(origin_key->seq,origin_key->type,origin_key->user_key,"");
    r->pending_handle.SetBlockHandle(handle);
    r->pending_index_entry = true;

}

void TableBuilder::WriteBlock(BlockBuilder *block,BlockHandle* handle) {
    // File format contains a sequence of blocks where each block has:
    //    block_data: uint8[n]

    Rep* r = rep_;
    Slice raw = block->Finish();
    WriteRawBlock(raw, handle);
    block->Reset();
}
void TableBuilder::WriteRawBlock(const Slice& block_contents,BlockHandle* handle) {
    Rep* r = rep_;
    handle->set_offset(r->offset);
    handle->set_size(block_contents.size());
    bool ret = r->file->Append(block_contents);
    r->offset+= block_contents.size();
    assert(ret);
}

#endif //KVENGINE_TABLE_BUILDER_H
