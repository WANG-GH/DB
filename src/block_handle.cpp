//
// Created by Linfp on 2021/4/20.
//

#include "block_handle.h"

void Footer::EncodeTo(std::string *dst) const {
    const size_t original_size = dst->size();
    index_handle_.EncodeTo(dst);
    //小端序
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
    Status  result = index_handle_.DecodeFrom(input);
    if (result.ok()) {
        // We skip over any leftover data (infact nothing for now) in "input"
        const char* end = magic_ptr + 8;
        *input = Slice(end, input->data() + input->size() - end);
    }
    return result;

}