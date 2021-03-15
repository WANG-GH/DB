//
// Created by 彭琳峰 on 2020-10-24.
//

#ifndef MYDB_MYDB_H
#define MYDB_MYDB_H

#include <iostream>
#include <vector>
#include <atomic>
#include "../file/file.h"
#include "../include/comparator.h"
#include "../include/env.h"
#include "../util/skiplist.h"
#include "../util/hash.h"
#define MAX_DATA_SIZE 4194304 //4M
#define likely(x) __builtin_expect(!!(x), 1) //gcc内置函数, 帮助编译器分支优化
#define unlikely(x) __builtin_expect(!!(x), 0)

const std::string kIndexFilePrefix = "/index-";
const std::string kDataFilePrefix = "/data-";

// Table store the indexs of data
typedef SkipList<Index, Compare> Table;



class MyDB {
private:
    std::string dbname_;
    int file_id_;
    int file_offset_;
    //硬盘存储
    int max_files_num_;   //文件数目
    std::vector<PosixFile *> data_files_;
    std::vector<PosixFile *> index_files_;
    //内存存储
    Table * table;
public:
    MyDB(const std::string &dbname) : dbname_(dbname), max_files_num_(5) {
        file_id_ = 0;
        table = NULL;
    }

    ~MyDB() {
        for (auto file : data_files_) {
            delete file;
        }
        for (auto file : index_files_) {
            delete file;
        }
        delete table;
    }

public:



    bool Put(const std::string &key, const std::string &value);

    bool Get(const std::string &key, std::string *ret_value);

    void LoadIndexFromFile(PosixFile *file);

    bool CallBackIndex();

    //声明Open为static函数，对全局暴露
    static bool Open(const std::string &dbname, MyDB **pdb);

    void SetWritePos(int file_id, int offset);
};


#endif //MYDB_MYDB_H
