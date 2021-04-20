//
// Created by yeye on 2021/4/13.
//

#ifndef KVENGINE_ENV_H
#define KVENGINE_ENV_H

#include <string>
#include <vector>
#include "slice.h"
class RandomAccessFile;
class WritableFile;
class FileState;
class Env{
public:
    virtual bool NewWritableFile(const std::string& fname,
                                   WritableFile** result) = 0;
    virtual bool NewRandomAccessFile(const std::string& fname,
                               RandomAccessFile** result) = 0;
    virtual bool CreateDir(std::string& dirname) = 0;
    virtual bool FileExists(std::string& fname) = 0;
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
    bool Read(uint64_t offset, size_t n, Slice* result, char* scratch) const;
    bool Append(const Slice& data);
    size_t DebugBlockSize();
private:
    const int kBlockSize = 8 * 1024;
    int refs_;
    std::vector<char*> blocks_;
    uint64_t size_;
};

class RandomAccessFile{
public:
    RandomAccessFile() = default;
    RandomAccessFile(const RandomAccessFile&) = delete;
    RandomAccessFile& operator=(const RandomAccessFile&) = delete;
    virtual ~RandomAccessFile() = default;

    virtual bool Read(uint64_t offset, size_t n, Slice* result,
                        char* scratch) const = 0;
};

class MemRandomAccessFile: public RandomAccessFile{
public:
    MemRandomAccessFile(FileState* file): file_(file){file_->Ref();} ;
    ~MemRandomAccessFile() override;

    bool Read(uint32_t offset, size_t n, Slice* result,
                char* scratch) const ;
private:
    FileState* file_;
};

class WritableFile{
public:
    WritableFile()= default;
    WritableFile(const WritableFile&) = delete;
    WritableFile& operator=(const WritableFile&) = delete;
    virtual ~WritableFile() = default;

    virtual bool Append(const Slice& data) = 0;
    virtual bool Flush() = 0;
    virtual bool Close() = 0;
};

class MemWritableFile: public WritableFile{
public:
    MemWritableFile(FileState* file): file_(file){file->Ref();}
    ~MemWritableFile() { file_->Unref(); }

    bool Append(const Slice& data);
    //bool Flush();
    //bool Close();

private:
    FileState* file_;
};

class MemEnv : public Env{
    bool NewWritableFile(const std::string& fname,
                                   WritableFile** result) override{return true;};
    bool NewRandomAccessFile(const std::string& fname,
                                       RandomAccessFile** result) override{return true;};
    bool CreateDir(std::string& dirname)  override{return true;};
    bool FileExists(std::string& fname) override{return true;};
};

class PosixEnv : public Env{
    bool NewWritableFile(const std::string& fname,
                           WritableFile** result) override;
    bool NewRandomAccessFile(const std::string& fname,
                               RandomAccessFile** result) override;
    bool CreateDir(std::string& dirname)  override;
    bool FileExists(std::string& fname) override;
};

const int kBufferSize = 1024*64;
class PosixWritableFile: public WritableFile{
public:
    PosixWritableFile(int fd): fd_(fd){}
    ~PosixWritableFile();
    bool Append(const Slice& data) override;
    bool Flush() override;
    bool Close() override;
private:
    bool FlushBuffer();
    bool WriteUnbuffered(const char* data, size_t size);
    int fd_;
    std::string dbname_;
    std::string dirname_;
    size_t pos_;
    char buf_[kBufferSize];
};

class PosixRandomAccessFile: public RandomAccessFile{
public:
    PosixRandomAccessFile(int fd, const std::string& fname):fd_(fd), filename_(fname){};
    ~PosixRandomAccessFile();

    bool Read(uint64_t offset, size_t n, Slice* result,
                      char* scratch) const;

private:
    const int fd_;
    std::string filename_;
};
#endif //KVENGINE_ENV_H
