//
// Created by Linfp on 2021/4/19.
//

#ifndef KVENGINE_SSTABLE_H
#define KVENGINE_SSTABLE_H

#include "block.h"
#include "block_handle.h"
#include "env.h"
#include "iterator.h"
class SSTable{

public:
    SSTable(std::string filename){
        //open the file
        //file = open(filename)
        //get the file stat
        //footer_size = footer->size()
    }
    Slice Get(Slice& key){
    }
    Block* readBlock(BlockHandle blockHandle){
        char * m = new char[blockHandle.size_];

        return Block(buf);
    }

private:
    Block* index;
    Footer* footer;
    WritableFile* file;
};
class SStableIterator: public Iterator{

public:
    SStableIterator(){}


    virtual bool Valid() const {
        return data_iterator!=NULL && data_iterator->Valid();
    }

    InternalKey* InternalKey(){
        return  data_iterator->InternalKey();
    }
    Slice Key(){
        return data_iterator->InternalKey()->user_key;
    }
    Slice value(){
        return data_iterator->InternalKey()->user_value;
    }
    virtual void SeekToFirst() {
     index_iterator->SeekToFirst();

    }

    virtual void SeekToLast() {

    }
    virtual void Seek(const Slice& target) {

    }

    virtual void Next() {
        data_iterator->Next();
        while (data_iterator==NULL||!data_iterator->Valid()){
            if (!index_iterator->Valid()){
                data_iterator = NULL;
                return;
            }
            index_iterator->Next();

        }
    }

    virtual void Prev() {

    }

    virtual Slice key() const {

    }

    virtual Slice value() const {

    }

private:
    SSTable * table;
    BlockHandle  data_block_handle;
    BlockIterator *data_iterator;
    BlockIterator *index_iterator;
    void init_data_block(){
        if (!index_iterator->Valid()){
            data_iterator = NULL;
        } else{
            IndexBlockHandle indexBlockHandle;
            indexBlockHandle.internalKey = index_iterator->InternalKey();
            BlockHandle tmpBlockHandle;
            indexBlockHandle.GetBlockHandle(&tmpBlockHandle);
            if (data_iterator!=NULL&&data_block_handle==tmpBlockHandle){
                // data_iter_ is already constructed with this iterator, so
                // no need to change anything
            }else{
                data_iterator = table.
            }
        }
    }
}

#endif //KVENGINE_SSTABLE_H
