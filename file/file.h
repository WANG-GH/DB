//
// Created by 彭琳峰 on 2020-10-22.
//

#ifndef MYDB_FILE_H
#define MYDB_FILE_H


#include <cstdint>
#include <cstdio>

#include "../util/coding.h"
#include "../util/spinlock.h"

class Index;

class Data;

class PosixFile {
private:
    FILE *fp_;
    std::string filename_;
    std::atomic<int> offset_;
    SpinLock file_lock_;
public:
    PosixFile() = delete;

    ~PosixFile() {
        Close();
    }

    PosixFile(FILE *fp, const std::string file_name, int offset) :
            fp_(fp), filename_(file_name), offset_(offset) {
    }

public:
    //read n char from offset into n
    bool Read(char *buf, int offset, int n) {
        if (fp_) {
            fseek(fp_, offset, SEEK_SET);
            fread(buf, n, 1, fp_);
            return true;
        }
        return false;
    }

    //wirte n char from buf into file
    //make sure the offset is at SEEK_END
    bool Write(const char *buf, int n) {
        if (fp_) {
            offset_.fetch_add(n);
            fseek(fp_, offset_.load(), SEEK_SET);
            fwrite(buf, n, 1, fp_);
            return true;
        }
        return false;
    }

    bool AppendIndex(const Index &index) {
        int index_size = index.GetIndexSize() + sizeof(int);
        char buf[index_size];
        if (!buf) {
            return false;
        }
        EncodeIndex(buf, index);
        Write(buf, index_size);
        return true;
    }

    bool AppendData(const Data &data) {
        char buf[data.data_size_];
        if (!buf) {
            return false;
        }
        EncodeData(buf, data);
        Write(buf, data.data_size_);
        return true;
    }

    int GetOffset() {
        return offset_.load();
    }

    bool Close() {
        fclose(fp_);
        fp_ = nullptr;
        return true;
    }

};

#endif //MYDB_FILE_H
