//
// Created by 彭琳峰 on 2020-10-22.
//

#ifndef MYDB_ENV_H
#define MYDB_ENV_H

#include <zconf.h>
#include <sys/stat.h>
#include "../file/file.h"

class Env {
public:
    static bool  AddFile(std::string file_name, PosixFile **fp) {
        int offset;
        FILE *fptr = fopen(file_name.c_str(), "a+");
        offset = get_file_size(fptr);
        *fp = new PosixFile(fptr, file_name, offset);
        if (*fp == nullptr) return false;
        return true;
    }

    static bool GetCurrentDir(std::string *dir) {
        char cwd[125];
        if (getcwd(cwd, sizeof(cwd)) != nullptr) {
            *dir = std::string(cwd);
            return true;
        }
        return false;
    }

    //增加代码可读性
    static bool FileExists(const std::string &file_index) {
        if (access(file_index.c_str(), F_OK) == 0) {
            return true;
        }
        return false;
    }

    static bool CreateDir(const std::string &dir) {
        std::cout << "CreateDir : " << dir << std::endl;
        if (mkdir(dir.c_str(),0755) == 0) {  //创建成功返回0，否则返回-1
            return true;
        }
        return false;
    }
    static   int get_file_size(FILE * fp){
        struct stat statbuff;
        fstat(fileno(fp),&statbuff);
        return statbuff.st_size;
    }

};

#endif //MYDB_ENV_H
