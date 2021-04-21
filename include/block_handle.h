//
// Created by Linfp on 2021/4/18.
//

#ifndef KVENGINE_BLOCK_HANDLE_H
#define KVENGINE_BLOCK_HANDLE_H
#include "coding.h"
#include "dbformat.h"
#include "status.h"

static const uint64_t kTableMagicNumber = 0xdb4775248b80fb57ull;

class BlockHandle {
public:
    // Maximum encoding length of a BlockHandle
  //  enum { kMaxEncodedLength = 10 + 10 };  原版采用varint故这里是10+10
    enum { kMaxEncodedLength = 8 + 8 };
    inline BlockHandle():offset_(0),size_(0){}
    // The offset of the block in the file.
    uint64_t offset() const { return offset_; }
    void set_offset(uint64_t offset) { offset_ = offset; }
    // The size of the stored block
    uint64_t size() const { return size_; }
    void set_size(uint64_t size) { size_ = size; }
    void EncodeTo(std::string* str) const {
        // Sanity check that all fields have been set
        assert(offset_ != ~static_cast<uint64_t>(0));
        assert(size_ != ~static_cast<uint64_t>(0));
        PutFixed64(str,offset_);
        PutFixed64(str,size_);
    }
    Status DecodeFrom(Slice * input ){
        offset_ = DecodeFixed64(input->data());
        size_ = DecodeFixed64(input->data()+8);
        return Status();
    }

private:   //variables are public
    uint64_t offset_;
    uint64_t size_;
};

class IndexBlockHandle {

public:
    InternalKey * internalKey;

    void SetBlockHandle(BlockHandle blockHandle){
        std::string str;
        blockHandle.EncodeTo(&str);
        internalKey->user_value = str;
    }
    BlockHandle GetBlockHandle(){
        BlockHandle handle;
        handle.DecodeFrom(&internalKey->user_value);
        return handle;
    }
};

class Footer {
public:
    enum { kEncodedLength = BlockHandle::kMaxEncodedLength + 8 };
    Footer() = default;
    BlockHandle& index_handle()  { return index_handle_; }
    void set_index_handle(const BlockHandle& h) { index_handle_ = h; }
    void EncodeTo(std::string* dst) const;
    Status DecodeFrom(Slice* input);
private:
    BlockHandle index_handle_;
};





#endif //KVENGINE_BLOCK_HANDLE_H
