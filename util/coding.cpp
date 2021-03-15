//
// Created by 彭琳峰 on 2020-10-25.
//
#include <cstring>
#include "coding.h"

int DecodeFixedInt(const char *ptr) {
    int result;
    memcpy(&result, ptr, sizeof(int));
    return result;
}

void EncodeIndex(char *buf, const Index &idx) {
    int pos = 0;
    int idx_size = idx.GetIndexSize();
    memcpy(buf + pos, &idx_size, sizeof(int));
    pos += sizeof(int);
    memcpy(buf + pos, &idx.file_id_, sizeof(int));
    pos += sizeof(int);
    memcpy(buf + pos, &idx.offset_, sizeof(int));
    pos += sizeof(int);
    memcpy(buf + pos, &idx.key_size_, sizeof(int));
    pos += sizeof(int);
    memcpy(buf + pos, idx.key_.c_str(), idx.key_size_);
    pos += idx.key_size_;
    memcpy(buf + pos, &idx.value_size_, sizeof(int));
}

void DecodeIndex(char *buf,
                 std::string *key,
                 int *f_id,
                 int *offset,
                 int *key_size,
                 int *value_size) {
    int pos = 0;
    *f_id = DecodeFixedInt(buf + pos);
    pos += sizeof(int);
    *offset = DecodeFixedInt(buf + pos);
    pos += sizeof(int);
    *key_size = DecodeFixedInt(buf + pos);
    pos += sizeof(int);
    *key = std::string(buf + pos, *key_size);
    pos += *key_size;
    *value_size = DecodeFixedInt(buf + pos);
}

void EncodeData(char *buf, const Data &data) {

    int pos = 0;
    //key_size int
    memcpy(buf, data.key_.c_str(), data.key_size_);
    pos += data.key_size_;
    //value_size int
    memcpy(buf + pos, data.value_.c_str(), data.value_size_);
}

void DecodeData(char *buf, int key_size, int value_size, std::string *key, std::string *value) {
    *key = std::string(buf, key_size);
    *value = std::string(buf + key_size, value_size);
}
