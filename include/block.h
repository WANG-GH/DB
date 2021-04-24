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
#include "block_builder.h"

class BlockIterator;

class Block {
    friend class BlockIterator;

public:
    Block(Slice content) {
        buffer_ = content.data();
        length = content.size();
    }
    inline uint64_t GetSlotsNum(){
        return DecodeFixed64(buffer_+length-sizeof(uint64_t));
    }
    inline const Slot* GetSlotsStartPointer(){
        return reinterpret_cast<const Slot*>(buffer_ + length - sizeof(uint64_t) - GetSlotsNum()*sizeof(Slot));
    }
    Iterator *NewIterator();

private:
    const char * buffer_;
    size_t length;
};

class BlockIterator : public Iterator {
public:
    explicit BlockIterator(Block *block_) {
        block = block_;
        id = 0;
        slots = block->GetSlotsStartPointer();
    }

    ~BlockIterator() override {
        delete block;
    }

    bool Valid() const override {
        return (id >= 0) && (id <block->GetSlotsNum());
    }

    void SeekToFirst() override {
        id = 0;
    }

    void SeekToLast() override {
        id = block->GetSlotsNum()-1;
    }

    //binary search, find the key >= target
    void Seek(const Slice &target) override {
        struct SlotComparator{
            const char *buffer; // buffer指针
            int operator()(const Slot& slot,const Slice& target){
                uint32_t key_size =  DecodeFixed32(buffer+slot.offset);
                Slice key(buffer+slot.offset+sizeof(uint32_t), key_size);
                return key.compare(target);
            }
        };
        SlotComparator comparator{};
        comparator.buffer = block->buffer_;
        int left = 0, right = block->GetSlotsNum()-1;
        while (left < right) {
            int mid = (left + right)/2;
            int ret = comparator(slots[mid],target);
            if (ret>0) {
                right = mid - 1;
            } else if (ret <0) {
                left = mid + 1;
            } else {
                id = mid;
                return;
            }
        }
        if (comparator(slots[left],target) == 0) {
            id = left;
            return;
        }
            //not found
        else {
            id = block->GetSlotsNum()-1;
            return;
        }
    }

    void Next() override {
        id++;
    }

    void Prev() override {
        id--;
    }

    Slice key() const override {
        return  getKeyFromSlot(slots[id]);
    }

    Slice value() const override {

        return getValueFromSlot(slots[id]);
    }

private:
    Slice getKeyFromSlot(const Slot& slot) const {
        uint32_t key_size =  DecodeFixed32(block->buffer_+slot.offset);
        return Slice(block->buffer_+slot.offset+sizeof(uint32_t), key_size);
    }
    Slice getValueFromSlot(const Slot& slot)const {
        uint32_t key_size =  DecodeFixed32(block->buffer_+slot.offset);
        uint32_t val_size =DecodeFixed32(block->buffer_+slot.offset+
                    sizeof(uint32_t)+key_size);
        return Slice(block->buffer_+slot.offset+
                    sizeof(uint32_t)+key_size+sizeof(uint32_t), val_size);
    }
    Block *block;
    const Slot * slots;
    int id;
};



#endif //KVENGINE_BLOCK_H
