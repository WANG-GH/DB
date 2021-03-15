//
// Created by 彭琳峰 on 2020-10-23.
//

#ifndef MYDB_DATA_H
#define MYDB_DATA_H

#include<iostream>

class Data {
public:
    //用一个公用方法来返回这些成员太麻烦了，不如直接声明为共有成员。其实写成结构体可能好点
    std::string key_;
    std::string value_;
    int key_size_;
    int value_size_;
    int data_size_;
public:
    Data(std::string key, std::string value) {
        key_ = key;
        value_ = value;
        key_size_ = key.size();
        value_size_ = value.size();
        data_size_ = key_size_ + value_size_;
    }

    ~Data() {}

};


#endif //MYDB_DATA_H
