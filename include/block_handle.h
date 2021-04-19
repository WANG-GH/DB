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
    void EncodeTo(std::string* str) const {
        // Sanity check that all fields have been set
        assert(offset_ != ~static_cast<uint64_t>(0));
        assert(size_ != ~static_cast<uint64_t>(0));
        PutFixed64(str,offset_);
        PutFixed64(str,size_);
    }



    Status DecodeFrom(Slice * input ){
        assert(input->size()==16);
        offset_ = DecodeFixed64(input->data());
        size_ = DecodeFixed64(input->data()+8);
    }

public:   //variables are public
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
    void GetBlockHandle(BlockHandle* blockHandle){
        blockHandle->DecodeFrom(&internalKey->user_value);
    }
};

class Footer {
public:
    enum { kEncodedLength = 2 * BlockHandle::kMaxEncodedLength + 8 };
    const BlockHandle& metaindex_handle() const { return metaindex_handle_; }
    void set_metaindex_handle(const BlockHandle& h) { metaindex_handle_ = h; }

    BlockHandle& index_handle()  { return index_handle_; }
    void set_index_handle(const BlockHandle& h) { index_handle_ = h; }
    Footer() = default;
    void EncodeTo(std::string* dst) const;
    Status DecodeFrom(Slice* input);
private:
    BlockHandle metaindex_handle_;
    BlockHandle index_handle_;

};

void Footer::EncodeTo(std::string *dst) const {
    const size_t original_size = dst->size();
    metaindex_handle_.EncodeTo(dst);
    index_handle_.EncodeTo(dst);
    dst->resize(2 * BlockHandle::kMaxEncodedLength);  // Padding
    PutFixed32(dst, static_cast<uint32_t>(kTableMagicNumber & 0xffffffffu));
    PutFixed32(dst, static_cast<uint32_t>(kTableMagicNumber >> 32));
    assert(dst->size() == original_size + kEncodedLength);
    (void)original_size;  // Disable unused variable warning.
}

Status Footer::DecodeFrom(Slice *input) {
    const char* magic_ptr = input->data() + kEncodedLength - 8;
    const uint32_t magic_lo = DecodeFixed32(magic_ptr);
    const uint32_t magic_hi = DecodeFixed32(magic_ptr + 4);
    const uint64_t magic = ((static_cast<uint64_t>(magic_hi) << 32) |
                            (static_cast<uint64_t>(magic_lo)));
    if (magic != kTableMagicNumber) {
        return Status::Corruption("not an sstable (bad magic number)");
    }

    Status result = metaindex_handle_.DecodeFrom(input);
    if (result.ok()) {
        result = index_handle_.DecodeFrom(input);
    }
    if (result.ok()) {
        // We skip over any leftover data (just padding for now) in "input"
        const char* end = magic_ptr + 8;
        *input = Slice(end, input->data() + input->size() - end);
    }
    return result;

}

#endif //KVENGINE_BLOCK_HANDLE_H
