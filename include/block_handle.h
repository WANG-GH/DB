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
    //    uint64_t offset_;
    //    uint64_t length_;
    enum { kMaxEncodedLength = 8 + 8 };
    inline BlockHandle():offset_(0),length_(0){}
    // The offset of the block in the file.
    uint64_t offset() const { return offset_; }
    void set_offset(uint64_t offset) { offset_ = offset; }
    // The size of the stored block
    uint64_t length() const { return length_; }
    void set_length(uint64_t len) { length_ = len; }
    void EncodeTo(std::string* str) const {
        // Sanity check that all fields have been set
        assert(offset_ != ~static_cast<uint64_t>(0));
        assert(length_ != ~static_cast<uint64_t>(0));
        PutFixed64(str,offset_);
        PutFixed64(str,length_);
    }
    Status DecodeFrom(Slice * input ){
        offset_ = DecodeFixed64(input->data());
        length_ = DecodeFixed64(input->data()+8);
        return Status();
    }

private:   //variables are public
    uint64_t offset_;
    uint64_t length_;
};



class Footer {
public:
    // Encoded length of a Footer.  Note tat the serialization of a
    // Footer will always occupy exactly this many bytes.  It consists
    // of two block handles and a magic number.
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
