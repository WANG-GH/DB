//
// Created by Linfp on 2021/4/13.
//

#ifndef KVENGINE_ITERATOR_H
#define KVENGINE_ITERATOR_H
#include "slice.h"
class Block;

class Iterator {
public:
        Iterator(){}

        Iterator(const Iterator&) = delete;
        Iterator& operator=(const Iterator&) = delete;

        virtual ~Iterator(){}

        virtual bool Valid() const = 0;

        virtual void SeekToFirst() = 0;

        virtual void SeekToLast() = 0;

        virtual void Seek(const Slice& target) = 0;

        virtual void Next() = 0;

        virtual void Prev() = 0;

    virtual Slice key() const = 0;

    virtual Slice value() const = 0;

};


class BlockIterator:public Iterator{
public:
    BlockIterator(Block* block_){
        block=block_;
        id = 0;
    }
    virtual ~BlockIterator(){
        delete block;
    }
   InternalKey* InternalKey(){
        return block->items[id];
    }
    bool Valid() const{
        return (id>=0) && (id<block->items.size());
    }

 void SeekToFirst() {
        id=0;
    }

    void SeekToLast() {
        id = block->items.size();
    }
    //binary search, find the key >= target
 void Seek(const Slice& target) {
        int left = 0,right=block->items.size()-1;
        while (left<right){
            int mid = left+right>>1;
            if (block->items[mid]->user_key.compare(target) > 0){
                right = mid-1;
            }
            else if(block->items[mid]->user_key.compare(target) < 0){
                left = mid+1;
            }
                    else{
                id = mid;
                return ;
            }
        }
        if (block->items[left]->user_key.compare(target)==0){
            id = left;
            return ;
        }
            //not found
        else{
            id = block->items.size();
            return ;
        }
    }

   void Next() {
        id++;
    }

    void Prev() {
        id--;
    }

   Slice key()const{
        return block->items[id]->user_key;
    }
    Slice value()const{
        return block->items[id]->user_value;
    }

private:
    Block * block;
    int id;
};



#endif //KVENGINE_ITERATOR_H
