//
// Created by yeye on 2021/4/13.
//

#ifndef KVENGINE_ENV_H
#define KVENGINE_ENV_H

#include <string>
#include <vector>
#include <fcntl.h>
#include "status.h"

// Common flags defined for all posix open operations
#if defined(HAVE_O_CLOEXEC)
constexpr const int kOpenBaseFlags = O_CLOEXEC;
#else
constexpr const int kOpenBaseFlags = 0;
#endif  // defined(HAVE_O_CLOEXEC)
constexpr const size_t kWritableFileBufferSize = 65536;

class Env{
public:
    Status CreateDir(std::string& dirname);
    Status FileExists(std::string& fname);
};

class FileState{
public:
    FileState(): refs_(0), size_(0) {}
    FileState(const FileState&) = delete;
    FileState& operator=(const FileState&) = delete;

    void Ref();
    void Unref();
    uint64_t Size() const;
    void Truncate();
    Status Read(uint64_t offset, size_t n, Slice* result, char* scratch) const;
    Status Append(const Slice& data);
private:
    const int kBlockSize = 8 * 1024;
    int refs_;
    std::vector<char*> blocks_;
    uint64_t size_;
};

class RandomAccessFile{
public:
    RandomAccessFile(FileState* file): file_(file){file_->Ref();} ;
    RandomAccessFile(const RandomAccessFile&) = delete;
    RandomAccessFile& operator=(const RandomAccessFile&) = delete;
    ~RandomAccessFile();

    Status Read(uint32_t offset, size_t n, Slice* result,
                char* scratch) const ;

private:
    FileState* file_;
};


class WritableFile{
public:
    WritableFile(FileState* file): file_(file){file->Ref();}
    ~WritableFile() { file_->Unref(); }

    Status Append(const Slice& data);
    Status Flush();
    Status Close();

private:
    FileState* file_;
};


#endif //KVENGINE_ENV_H
