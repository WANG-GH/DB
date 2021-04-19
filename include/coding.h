//
// Created by Linfp on 2021/4/9.
//

#ifndef KVENGINE_CODING_H
#define KVENGINE_CODING_H

#include <cstdint>

inline char* EncodeFixed32(char* dst, uint32_t value) {
    uint8_t* const buffer = reinterpret_cast<uint8_t*>(dst);

    // Recent clang and gcc optimize this to a single mov / str instruction.
    buffer[0] = static_cast<uint8_t>(value);
    buffer[1] = static_cast<uint8_t>(value >> 8);
    buffer[2] = static_cast<uint8_t>(value >> 16);
    buffer[3] = static_cast<uint8_t>(value >> 24);
    return dst+4;
}

inline char* EncodeFixed64(char* dst, uint64_t value) {
    uint8_t* const buffer = reinterpret_cast<uint8_t*>(dst);

    // Recent clang and gcc optimize this to a single mov / str instruction.
    buffer[0] = static_cast<uint8_t>(value);
    buffer[1] = static_cast<uint8_t>(value >> 8);
    buffer[2] = static_cast<uint8_t>(value >> 16);
    buffer[3] = static_cast<uint8_t>(value >> 24);
    buffer[4] = static_cast<uint8_t>(value >> 32);
    buffer[5] = static_cast<uint8_t>(value >> 40);
    buffer[6] = static_cast<uint8_t>(value >> 48);
    buffer[7] = static_cast<uint8_t>(value >> 56);
    return dst+8;
}

inline uint32_t DecodeFixed32(const char* ptr) {
    const uint8_t* const buffer = reinterpret_cast<const uint8_t*>(ptr);

    // Recent clang and gcc optimize this to a single mov / ldr instruction.
    return (static_cast<uint32_t>(buffer[0])) |
           (static_cast<uint32_t>(buffer[1]) << 8) |
           (static_cast<uint32_t>(buffer[2]) << 16) |
           (static_cast<uint32_t>(buffer[3]) << 24);
}

inline uint64_t DecodeFixed64(const char* ptr) {
    const uint8_t* const buffer = reinterpret_cast<const uint8_t*>(ptr);

    // Recent clang and gcc optimize this to a single mov / ldr instruction.
    return (static_cast<uint64_t>(buffer[0])) |
           (static_cast<uint64_t>(buffer[1]) << 8) |
           (static_cast<uint64_t>(buffer[2]) << 16) |
           (static_cast<uint64_t>(buffer[3]) << 24) |
           (static_cast<uint64_t>(buffer[4]) << 32) |
           (static_cast<uint64_t>(buffer[5]) << 40) |
           (static_cast<uint64_t>(buffer[6]) << 48) |
           (static_cast<uint64_t>(buffer[7]) << 56);
}

//void PutFixed32(std::string* dst, uint32_t value) {
//    char buf[sizeof(value)];
//    EncodeFixed32(buf, value);
//    dst->append(buf, sizeof(buf));
//}
//
//void PutFixed64(std::string* dst, uint64_t value) {
//    char buf[sizeof(value)];
//    EncodeFixed64(buf, value);
//    dst->append(buf, sizeof(buf));
//}

inline char* Copy(char*dst,const char *src,uint64_t len){
    memcpy(dst,src,len);
    return dst+len;
}

#endif //KVENGINE_CODING_H
