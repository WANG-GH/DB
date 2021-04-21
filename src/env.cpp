//
// Created by yeye on 2021/4/13.
//

#include "env.h"

bool PosixEnv::CreateDir(std::string &dirname) {
    DIR* dir;
    dir = opendir(dirname.c_str());
    //TODO: return false when dir is exist
    if(dir == nullptr)
    {
        std::string path = "./" + dirname;
        mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
        return true;
    }
    return false;
}

bool PosixEnv::FileExists(std::string &fname) {
    bool s;
    if(access(fname.c_str(), F_OK) == 0)
        return true;
    return false;
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
bool FileState::Read(uint64_t offset, size_t n, Slice *result, char *scratch) const {
    if (offset > size_) {
        return false;
    }
    //judge whether the n is at the end of file.
    const uint64_t available = size_ - offset;
    if (n > available) {
        n = static_cast<size_t>(available);
    }
    if (n == 0) {
        *result = Slice();
        return true;
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
    return true;
}

bool FileState::Append(const Slice &data) {
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

        memcpy(blocks_.back() + offset, src, avail);
        src_len -= avail;
        src += avail;
        size_ += avail;
    }

    return true;
}

size_t FileState::DebugBlockSize() {
    return blocks_.size();
}

bool MemRandomAccessFile::Read(uint32_t offset, size_t n,
                              Slice *result, char *scratch) const {
    return file_->Read(offset, n, result,scratch);
}

MemRandomAccessFile::~MemRandomAccessFile() {
    file_->Unref();
}

bool MemWritableFile::Append(const Slice &data) {
    return file_->Append(data);
}

bool PosixWritableFile::Append(const Slice &data) {
    size_t write_size = data.size();
    const char* write_data = data.data();

    size_t copy_size = std::min(write_size, kBufferSize - pos_);
    std::memcpy(buf_ + pos_, write_data, copy_size);
    write_data += copy_size;
    write_size -= copy_size;
    pos_ += copy_size;
    if (write_size == 0) {
        return true;
    }

    // Can't fit in buffer, so need to do at least one write.
    bool status = FlushBuffer();
    if (!status) {
        return status;
    }

    // Small writes go to buffer, large writes are written directly.
    if (write_size < kBufferSize) {
        std::memcpy(buf_, write_data, write_size);
        pos_ = write_size;
        return true;
    }
    return WriteUnbuffered(write_data, write_size);
}

bool PosixWritableFile::WriteUnbuffered(const char *data, size_t size) {
    while (size > 0) {
        ssize_t write_result = ::write(fd_, data, size);
        if (write_result < 0) {
            if (errno == EINTR) {
                continue;  // Retry
            }
            return false;
        }
        data += write_result;
        size -= write_result;
    }
    return true;
}

bool PosixWritableFile::Flush() {
    return FlushBuffer();
}

bool PosixWritableFile::FlushBuffer() {
    bool status = WriteUnbuffered(buf_, pos_);
    pos_ = 0;
    return status;
}

PosixWritableFile::~PosixWritableFile() {
    if(fd_ > 0)
    Close();
}

bool PosixWritableFile::Close() {
    Flush();
    close(fd_);
    return true;
}

bool PosixEnv::NewWritableFile(const std::string &fname, WritableFile **result) {
    int fd = open(fname.c_str(), O_TRUNC | O_WRONLY | O_CREAT, 0664);
    *result = nullptr;
    if(fd > 0)
    {
        *result = new PosixWritableFile(fd);
        return true;
    }
    return false;
}

bool PosixEnv::NewRandomAccessFile(const std::string &fname, RandomAccessFile **result) {
    int fd = open(fname.c_str(), O_RDONLY);
    *result = nullptr;
    if(fd > 0){
        *result = new PosixRandomAccessFile(fd, fname);
        return true;
    }
    return false;
}

bool PosixEnv::NewSequentialFile(const std::string &f, SequentialFile **r) {
    int fd = open(f.c_str(), O_RDONLY);
    if(fd > 0){
        *r = new PosixSequentialFile(f, fd);
        return true;
    }
    return false;
}

bool PosixRandomAccessFile::Read(uint64_t offset, size_t n, Slice *result, char *scratch) const {
    lseek(fd_, offset, SEEK_SET);
    size_t length = read(fd_, scratch, n);
    if(length > 0){
        *result = Slice(scratch, n);
        return true;
    }
    result = nullptr;
    return false;
}

PosixRandomAccessFile::~PosixRandomAccessFile() {
    close(fd_);
}

bool PosixSequentialFile::Read(size_t n, Slice *result, char *scratch) {
    bool status;
    while (true) {
        ssize_t read_size = ::read(fd_, scratch, n);
        if (read_size < 0) {  // Read error.
            if (errno == EINTR) {
                continue;  // Retry
            }
            status = false;
            break;
        }
        *result = Slice(scratch, read_size);
        break;
    }
    return status;
}

bool PosixSequentialFile::Skip(uint64_t n) {
    if (lseek(fd_, n, SEEK_CUR) == static_cast<off_t>(-1)) {
        return false;
    }
    return true;
}
