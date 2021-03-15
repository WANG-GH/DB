//
// Created by 彭琳峰 on 2020-10-22.
// 关于这个文件，吸取一个惨痛的教训
// ！！！不要把全局变量的<定义>放在头文件里！！！
// 出了 multiple definition of XXX 的错，最开始
// 检查了头文件保护不是，又检查了类与类之间循环嵌套声明
// 最后花了很多时间，吸取教训
// 另外，如果要实现的全局函数很短小，确实想定义声明放一起
// 可以通过声明一个类然后将要实现的函数写成公有的静态方法，
// 来解决这个问题

#ifndef MYDB_CODING_H
#define MYDB_CODING_H

#include<iostream>
#include <cassert>
#include "../include/index.h"    //应该避免使用相对路径，但我这里用绝对路径不知道为什么找不到头文件...只能先这样写着了
#include "../include/data.h"

//Decode a int from ptr
int DecodeFixedInt(const char *ptr);

//Encoding and Decoding Index
void EncodeIndex(char *buf, const Index &index);

void DecodeIndex(char *buf,
                 std::string *key,
                 int *f_id,
                 int *offset,
                 int *key_size,
                 int *value_size);

void EncodeData(char *buf, const Data &data);

void DecodeData(char *buf, int key_size, int value_size,
                std::string *key, std::string *value);

#endif //MYDB_CODING_H
