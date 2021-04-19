//
// Created by Linfp on 2021/4/13.
//

#ifndef KVENGINE_BLOCK_H
#define KVENGINE_BLOCK_H

#include <vector>
//#include "key.h"
#include "dbformat.h"
#include "slice.h"
#include "coding.h"

class BlockIterator;

int32_t DecodeFixed32(const char * buf);

class Block {
    friend class BlockIterator;
public:
    Block(Slice content){
        int32_t counter=DecodeFixed32(content.data()+content.size()-4);
        for (int32_t i = 0,pos=0; i <counter ; ++i) {
            InternalKey *item = new InternalKey();
            pos+=item->DecodeFrom(content.data()+pos);
            items.push_back(item);
        }
    }
private:
    std::vector<InternalKey*> items;
};


#endif //KVENGINE_BLOCK_H
