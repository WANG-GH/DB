//
// Created by 彭琳峰 on 2020-10-22.
//

#ifndef MYDB_INDEX_H
#define MYDB_INDEX_H

#include<iostream>

class Index {
public:
    Index() {}

    //实现一个之传入key的构造函数，用来传入跳表
    //因为在跳表里搜索的时候只知道index的key
    Index(const std::string &key) : key_(key) {
    }

    Index(const std::string &key,
          int file_id,
          int offset,
          int key_size,
          int value_size)
            : key_(key), file_id_(file_id), offset_(offset), key_size_(key_size), value_size_(value_size) {
    }

public:
    int GetIndexSize() const {
        return key_.size() + sizeof(int) * 4;
    }

public:
    //用一个公用方法来返回这些成员太麻烦了，不如直接声明为共有成员，当成结构体来用
    std::string key_;
    int key_size_;
    int value_size_;
    int file_id_;
    int offset_;
    //maybe add a moddify date here
};

#endif //MYDB_INDEX_H
