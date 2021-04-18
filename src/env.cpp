//
// Created by yeye on 2021/4/13.
//

#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "env.h"

Status Env::CreateDir(std::string &dirname) {
    DIR* dir;
    dir = opendir(dirname.c_str());
    //TODO: return false when dir is exist
    if(dir == nullptr)
    {
        std::string path = "./" + dirname;
        mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
        return Status::OK();
    }
    return Status::IOError("Dir exists");
}

Status Env::FileExists(std::string &fname) {
    Status s;
    if(access(fname.c_str(), F_OK) == 0)
        return Status::OK();
    return Status::IOError("FileExits");
}

void FileState::Ref() {
    refs_++;
}

void FileState::Unref() {
    refs_--;
    if(refs_ == 0)
        delete this;
}

uint64_t FileState::Size() const {
    return size_;
}

void FileState::Truncate() {
    for(char*& block : blocks_)
        delete[] block;
    blocks_.clear();
    size_ = 0;
}

//scratch is the result.data().
Status FileState::Read(uint64_t offset, size_t n, Slice *result, char *scratch) const {
    if (offset > size_) {
        return Status::IOError("Offset greater than file size.");
    }
    //judge whether the n is at the end of file.
    const uint64_t available = size_ - offset;
    if (n > available) {
        n = static_cast<size_t>(available);
    }
    if (n == 0) {
        *result = Slice();
        return Status::OK();
    }

    //to find which the bock the offset is in and how many space the BlockOffset has.
    size_t block = static_cast<size_t>(offset / kBlockSize);
    size_t block_offset = offset % kBlockSize;
    size_t bytes_to_copy = n;
    char* dst = scratch;

    //the same use as the loop in append.
    //elegant
    while (bytes_to_copy > 0) {
        size_t avail = kBlockSize - block_offset;
        if (avail > bytes_to_copy) {
            avail = bytes_to_copy;
        }
        std::memcpy(dst, blocks_[block] + block_offset, avail);

        bytes_to_copy -= avail;
        dst += avail;
        block++;
        block_offset = 0;
    }

    *result = Slice(scratch, n);
    return Status::OK();
}

Status FileState::Append(const Slice &data) {
    const char* src = data.data();
    size_t src_len = data.size();

    //use this loop to create the new block and write data until the src's data is done.
    while (src_len > 0)
    {
        size_t avail;
        size_t offset = size_ % kBlockSize;
        if(offset > 0){
            avail = kBlockSize - offset;
        }else{
            blocks_.push_back(new char[kBlockSize]);
            avail = kBlockSize;
        }

        avail = (avail > src_len) ? src_len : avail;

        memcpy(blocks_.back(), src, avail);
        src_len -= avail;
        src += avail;
        size_ += avail;
    }

    return Status::OK();
}

Status RandomAccessFile::Read(uint32_t offset, size_t n,
                              Slice *result, char *scratch) const {
    return file_->Read(offset, n, result,scratch);
}

RandomAccessFile::~RandomAccessFile() {
    file_->Unref();
}
