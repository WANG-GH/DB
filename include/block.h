//
// Created by Linfp on 2021/4/13.
//

#ifndef KVENGINE_BLOCK_H
#define KVENGINE_BLOCK_H

#include <vector>

#include "dbformat.h"
#include "slice.h"
#include "coding.h"
#include "iterator.h"

class BlockIterator;

class Block {
    friend class BlockIterator;

public:
    Block(Slice content) {
        int32_t counter = DecodeFixed32(content.data() + content.size() - 4);
        for (int32_t i = 0, pos = 0; i < counter; ++i) {
            InternalKey *item = new InternalKey();
            pos += item->DecodeFrom(content.data() + pos);
            items.push_back(item);
        }
    }

    Iterator *NewIterator();


private:
    std::vector<InternalKey *> items;
};

class BlockIterator : public Iterator {
public:
    BlockIterator(Block *block_) {
        block = block_;
        id = 0;
    }

    virtual ~BlockIterator() {
        delete block;
    }

    InternalKey *internal_key() {
        return block->items[id];
    }

    bool Valid() const {
        return (id >= 0) && (id < block->items.size());
    }

    void SeekToFirst() {
        id = 0;
    }

    void SeekToLast() {
        id = block->items.size();
    }

    //binary search, find the key >= target
    void Seek(const Slice &target) {
        int left = 0, right = block->items.size() - 1;
        while (left < right) {
            int mid = left + right >> 1;
            if (block->items[mid]->user_key.compare(target) > 0) {
                right = mid - 1;
            } else if (block->items[mid]->user_key.compare(target) < 0) {
                left = mid + 1;
            } else {
                id = mid;
                return;
            }
        }
        if (block->items[left]->user_key.compare(target) == 0) {
            id = left;
            return;
        }
            //not found
        else {
            id = block->items.size();
            return;
        }
    }

    void Next() {
        id++;
    }

    void Prev() {
        id--;
    }

    Slice key() const {
        return block->items[id]->user_key;
    }

    Slice value() const {
        return block->items[id]->user_value;
    }

private:
    Block *block;
    int id;
};



#endif //KVENGINE_BLOCK_H
