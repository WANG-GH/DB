//
// Created by Linfp on 2021/4/20.
//

#include "block.h"
Iterator *Block::NewIterator() {
    return new BlockIterator(this);
}
