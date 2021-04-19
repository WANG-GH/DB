//
// Created by yeye on 2021/4/19.
//

#include  "block.h"
int32_t DecodeFixed32(const char * buf){
    int32_t n;
    memcpy(&n,buf,4);
    return n;
}