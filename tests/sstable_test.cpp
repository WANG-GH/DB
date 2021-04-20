//
// Created by Linfp on 2021/4/20.
//
#include "gtest/gtest.h"
#include "table.h"
#include "table_builder.h"
TEST(MEMTABLE_TEST,TEST_1){
    WritableFile* file;
    NewWritableFile("hello.db",&file);
    TableBuilder();
}