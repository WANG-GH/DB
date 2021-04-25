//
// Created by Linfp on 2021/4/20.
//
#include <memtable.h>
#include "gtest/gtest.h"
#include "table.h"
#include "table_builder.h"
#include "env.h"
class BytewiseComparatorImpl : public Comparator {
public:
    BytewiseComparatorImpl() = default;

    const char *Name() const override { return "leveldb.BytewiseComparator"; }

    int Compare(const Slice &a, const Slice &b) const override {
        return a.compare(b);
    }

    void FindShortestSeparator(std::string *start,
                               const Slice &limit) const override {
        // Find length of common prefix
        size_t min_length = std::min(start->size(), limit.size());
        size_t diff_index = 0;
        while ((diff_index < min_length) &&
               ((*start)[diff_index] == limit[diff_index])) {
            diff_index++;
        }

        if (diff_index >= min_length) {
            // Do not shorten if one string is a prefix of the other
        } else {
            uint8_t diff_byte = static_cast<uint8_t>((*start)[diff_index]);
            if (diff_byte < static_cast<uint8_t>(0xff) &&
                diff_byte + 1 < static_cast<uint8_t>(limit[diff_index])) {
                (*start)[diff_index]++;
                start->resize(diff_index + 1);
                assert(Compare(*start, limit) < 0);
            }
        }
    }

    void FindShortSuccessor(std::string *key) const override {
        // Find first character that can be incremented
        size_t n = key->size();
        for (size_t i = 0; i < n; i++) {
            const uint8_t byte = (*key)[i];
            if (byte != static_cast<uint8_t>(0xff)) {
                (*key)[i] = byte + 1;
                key->resize(i + 1);
                return;
            }
        }
        // *key is a run of 0xffs.  Leave it alone.
    }
};

void BuildTable(const std::string& dbname, Env* env, const Options& options,Iterator* iter){
    WritableFile* file;
    env->NewWritableFile(dbname,&file);
    TableBuilder* builder = new TableBuilder(options,file);
    for (; iter->Valid() ; iter->Next()) {
        //iter->key 得到的是internalkey  iter->value 得到的是user_value
        builder->Add(iter->key(),iter->value());
    }
    builder->Finish();
    delete builder;
    file->Close();
    delete file;
    file = nullptr;
    delete iter;
}
TEST(SSTABLE_TEST,TEST_1){
    Env* e = new PosixEnv();
    WritableFile* file;
    e->NewWritableFile("hello.db",&file);
    Options options;
    options.comparator = new BytewiseComparatorImpl();
    TableBuilder table_builder(options,file);

    for(int i=0;i<10;i++){
        char * key = new char[100];
        char * value = new char[100];
        memcpy(key,std::string("key"+std::to_string(i)).data(),std::string("key"+std::to_string(i)).size());
        memcpy(value,std::string("value"+std::to_string(i)).data(),std::string("value"+std::to_string(i)).size());
        Slice k(key,std::string("key"+std::to_string(i)).size());
        Slice v(value,std::string("value"+std::to_string(i)).size());
        InternalKey internalKey(k,i,kTypeValue);
        //table 插入的是internalkey
        table_builder.Add(Slice(internalKey.Encode().data() ,internalKey.Encode().size()),value);
    }
    table_builder.Finish();
    RandomAccessFile* rfile;
    uint64_t file_size;
    //TODO：env需要一个GetFileSize
    struct stat statbuf;
    stat("hello.db",&statbuf);
    e->NewRandomAccessFile("hello.db",&rfile);
    Table* table ;
    Table::Open(options,rfile,statbuf.st_size,&table);
    Iterator* iter = table->NewIterator();
    for (int i = 0; i < 10; ++i) {

        InternalKey internalKey(std::string("key" + std::to_string(i)).data(),i,kTypeValue);
        iter->Seek(Slice(internalKey.Encode().data(),internalKey.Encode().size()));
        if (iter->Valid()){
            ASSERT_EQ(iter->value(),"value"+std::to_string(i));
        }
    }

}

TEST(SSTABLE_TEST, TEST_2) {
    Comparator *usr_comparator = new BytewiseComparatorImpl();
    const InternalKeyComparator internal_comparator(usr_comparator);
    MemTable *memtable = new MemTable(internal_comparator);
    MemTable *immutable;
    Options options;
    options.comparator = new BytewiseComparatorImpl();
    for (int i = 0; i < 10000; ++i) {
        char *key = new char[100];
        char *value = new char[100];
        memcpy(key, std::string("key" + std::to_string(i)).data(), std::string("key" + std::to_string(i)).size());
        memcpy(value, std::string("value" + std::to_string(i)).data(), std::string("value" + std::to_string(i)).size());
        Slice k(key, std::string("key" + std::to_string(i)).size());
        Slice v(value, std::string("value" + std::to_string(i)).size());
        memtable->Add(i, kTypeValue, k, v);
        immutable = memtable;
        memtable = new MemTable(internal_comparator);
        Iterator* iter = memtable->NewIterator();
        Env* e = new PosixEnv();
        //实际需要另外起一个线程将immutable刷盘
        std::string dbname = "hello.db1";
        if (memtable->ApproximateMemoryUsage()>options.write_buffer_size){
            BuildTable(dbname,e,options,iter);
            //打开sstable
            RandomAccessFile* rfile;
            uint64_t file_size;
            //TODO：env需要一个GetFileSize
            struct stat statbuf;
            stat(dbname.data(),&statbuf);
            e->NewRandomAccessFile(dbname,&rfile);
            Table* table ;
            Table::Open(options,rfile,statbuf.st_size,&table);
            ASSERT_NE(table, nullptr);
            iter = table->NewIterator();
            int i=0;
            for (; i < 10; ++i) {
                InternalKey internalKey(std::string("key" + std::to_string(i)).data(),i,kTypeValue);
                iter->Seek(Slice(internalKey.Encode().data(),internalKey.Encode().size()));
                if (iter->Valid()){
                    ASSERT_EQ(iter->value(),"value"+std::to_string(i));
                }
            }
            ASSERT_EQ(i,10); //确保db打开且经过读取测试
        }
    }

}