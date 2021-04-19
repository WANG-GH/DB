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

    TableBuilder(const Options &options,WritableFile* file ):
        closed(false),offset(0),num_entries(0),pending_index_entry(false){
    }

    TableBuilder(const TableBuilder &) = delete;

    TableBuilder &operator=(const TableBuilder &) = delete;

    ~TableBuilder();


    Status ChangeOptions(const Options &options);

    void Add(InternalKey* internalKey){
        if (status().ok()){
            if (pending_index_entry){
                index_block_builder.Add(pending_handle.internalKey);
                pending_index_entry = false;
            }
        }
        pending_handle.internalKey=internalKey;
        num_entries++;
        data_block_builder.Add(internalKey);
        if(data_block_builder.CurrentSizeEstimate()> MAX_BLOCK_SIZE){
            Flush();
        }
    }


    void Flush(){
        if (data_block_builder.IsEmpty()){
            return ;
        }
        InternalKey* origin_key = pending_handle.internalKey;
        pending_handle.internalKey = new InternalKey(origin_key->seq,origin_key->type,origin_key->user_key,"");
        pending_handle.SetBlockHandle(WriteBlock(&data_block_builder));
        pending_index_entry = true;
    }


    Status status() const{
        return s;
    }


    void Finish(){
        //write data block
        Flush();

        //write indexblock
        if(pending_index_entry){
            index_block_builder.Add(pending_handle.internalKey);
            pending_index_entry = false;
        }
        Footer footer;
        footer.index_handle() = WriteBlock(&index_block_builder);

        //write footer
        std::string footer_encoding;
        footer.EncodeTo(&footer_encoding);
        file->Append(footer_encoding);
        file->Close();
        return ;
    }


private:
    bool ok() const { return status().ok(); }

    BlockHandle WriteBlock(BlockBuilder *block) {
        Slice content = block->Finish();
        BlockHandle blockHandle;
        blockHandle.offset_ = offset;
        blockHandle.size_ = content.size();
        offset+=blockHandle.size_;
        s = file->Append(content);
        file->Sync();
        block->Reset();
        return blockHandle;
    }

    Options options;
    Options index_block_options;
    WritableFile* file;
    uint64_t offset;
    BlockBuilder data_block_builder;
    BlockBuilder index_block_builder;
    std::string last_key;
    int64_t num_entries;
    bool closed;  // Either Finish() or Abandon() has been called.
    bool pending_index_entry;
    IndexBlockHandle pending_handle;  // Handle to add to index block
    Status s;
};

#endif //KVENGINE_TABLE_BUILDER_H
