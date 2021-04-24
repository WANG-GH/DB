//
// Created by yeye on 2021/4/17.
//

#ifndef KVENGINE_TABLE_H
#define KVENGINE_TABLE_H
#include "options.h"

#include "status.h"

#include "block.h"
#include "block_handle.h"
#include "env.h"
#include "iterator.h"

Status ReadBlock(RandomAccessFile *file, const Options &options,
                 const BlockHandle &handle, Slice *result);
class TableIterator;
class Table{
    friend TableIterator;
public:
    static Status Open(const Options& options, RandomAccessFile* file,
                       uint64_t file_size, Table** table) ;

    Table(const Table&) = delete;
    Table& operator=(const Table&) = delete;
    Iterator* NewIterator() ;
    Iterator *GetDataIter(Slice& index_value);

private:
    struct Rep;
    Table(Rep* rep): rep_(rep){};
    Status InternalGet(const Slice &key,Slice * value);
    Rep* const rep_;

    //static Iterator* BlockReader(void* arg, const Slice& index_value)
};
class TableIterator: public Iterator{
public:
   TableIterator (Table* table);
    ~TableIterator () = default;
    virtual bool Valid() const {
        return  data_iter_ != nullptr &&  data_iter_ ->Valid();
    }

    virtual Slice key()const{
        assert(Valid());
        return data_iter_->key();
    }
    virtual Slice value()const{
        assert(Valid());
        return data_iter_->value();
    }
    virtual void SeekToFirst() {
        index_iter_->SeekToFirst();
        InitDataBlock();
        if (data_iter_!= nullptr) data_iter_->SeekToFirst();
        SkipEmptyDataBlocksForward();
    }

    virtual void SeekToLast() {
        index_iter_->SeekToLast();
        InitDataBlock();
        if (data_iter_ != nullptr) data_iter_->SeekToLast();
        SkipEmptyDataBlocksBackward();
    }
    virtual void Seek(const Slice& target) {
        index_iter_->Seek(target);
        InitDataBlock();
        if (data_iter_ != nullptr) data_iter_->Seek(target);
        SkipEmptyDataBlocksForward();
    }

    virtual void Next() {
        assert(Valid());
        data_iter_->Next();
        SkipEmptyDataBlocksForward();
    }

    virtual void Prev() {
        assert(Valid());
        data_iter_->Prev();
        SkipEmptyDataBlocksBackward();
    }



private:
    Table * table_;
    Iterator* index_iter_;
    Iterator*  data_iter_;  // May be nullptr
    std::string data_block_handle_;

    void SkipEmptyDataBlocksForward(){
        while (data_iter_ == nullptr || !data_iter_->Valid()) {
            // Move to next block
            if (!index_iter_->Valid()) {
                data_iter_ = nullptr;
                return;
            }
            index_iter_->Next();
            InitDataBlock();
            if (data_iter_!= nullptr) data_iter_->SeekToFirst();
        }
    }
    void SkipEmptyDataBlocksBackward(){
        while (data_iter_ == nullptr || !data_iter_->Valid()) {
            // Move to next block
            if (!index_iter_->Valid()) {
                data_iter_ = nullptr;
                return;
            }
            index_iter_->Prev();
            InitDataBlock();
            if (data_iter_!= nullptr) data_iter_->SeekToLast();
        }
    }
    void InitDataBlock() {
        if (!index_iter_->Valid()) {
            data_iter_ = nullptr;
        } else {
            Slice handle = index_iter_->value();
            if (data_iter_ != nullptr &&
                handle.compare(data_block_handle_) == 0) {
                // data_iter_ is already constructed with this iterator, so
                // no need to change anything
            } else {
                //

                Iterator* iter = table_->GetDataIter( handle);
                data_block_handle_.assign(handle.data(), handle.size());
                data_iter_ = iter;
            }
        }
    }
};


#endif //KVENGINE_TABLE_H
