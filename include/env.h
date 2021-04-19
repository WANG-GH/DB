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
Status PosixError(const std::string& context, int error_number) {
    if (error_number == ENOENT) {
        return Status::NotFound(context, std::strerror(error_number));
    } else {
        return Status::IOError(context, std::strerror(error_number));
    }
}
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

class  WritableFile {
        public:
        WritableFile() = default;

        WritableFile(const WritableFile&) = delete;
        WritableFile& operator=(const WritableFile&) = delete;

        virtual ~WritableFile();

        virtual Status Append(const Slice& data) = 0;
        virtual Status Close() = 0;
        virtual Status Flush() = 0;
        virtual Status Sync() = 0;
};
class PosixWritableFile final : public WritableFile {
public:
    PosixWritableFile(std::string filename, int fd)
            : pos_(0),
              fd_(fd),
              is_manifest_(IsManifest(filename)),
              filename_(std::move(filename)),
              dirname_(Dirname(filename_)) {}

    ~PosixWritableFile() override {
        if (fd_ >= 0) {
            // Ignoring any potential errors
            Close();
        }
    }

    Status Append(const Slice& data) override {
        size_t write_size = data.size();
        const char* write_data = data.data();

        // Fit as much as possible into buffer.
        size_t copy_size = std::min(write_size, kWritableFileBufferSize - pos_);
        std::memcpy(buf_ + pos_, write_data, copy_size);
        write_data += copy_size;
        write_size -= copy_size;
        pos_ += copy_size;
        if (write_size == 0) {
            return Status::OK();
        }

        // Can't fit in buffer, so need to do at least one write.
        Status status = FlushBuffer();
        if (!status.ok()) {
            return status;
        }

        // Small writes go to buffer, large writes are written directly.
        if (write_size < kWritableFileBufferSize) {
            std::memcpy(buf_, write_data, write_size);
            pos_ = write_size;
            return Status::OK();
        }
        return WriteUnbuffered(write_data, write_size);
    }

    Status Close() override {
        Status status = FlushBuffer();
        const int close_result = ::close(fd_);
        if (close_result < 0 && status.ok()) {
            status = PosixError(filename_, errno);
        }
        fd_ = -1;
        return status;
    }

    Status Flush() override { return FlushBuffer(); }

    Status Sync() override {
        // Ensure new files referred to by the manifest are in the filesystem.
        //
        // This needs to happen before the manifest file is flushed to disk, to
        // avoid crashing in a state where the manifest refers to files that are not
        // yet on disk.
        Status status = SyncDirIfManifest();
        if (!status.ok()) {
            return status;
        }

        status = FlushBuffer();
        if (!status.ok()) {
            return status;
        }

        return SyncFd(fd_, filename_);
    }

private:
    Status FlushBuffer() {
        Status status = WriteUnbuffered(buf_, pos_);
        pos_ = 0;
        return status;
    }

    Status WriteUnbuffered(const char* data, size_t size) {
        while (size > 0) {
            ssize_t write_result = ::write(fd_, data, size);
            if (write_result < 0) {
                if (errno == EINTR) {
                    continue;  // Retry
                }
                return PosixError(filename_, errno);
            }
            data += write_result;
            size -= write_result;
        }
        return Status::OK();
    }

    Status SyncDirIfManifest() {
        Status status;
        if (!is_manifest_) {
            return status;
        }

        int fd = ::open(dirname_.c_str(), O_RDONLY | kOpenBaseFlags);
        if (fd < 0) {
            status = PosixError(dirname_, errno);
        } else {
            status = SyncFd(fd, dirname_);
            ::close(fd);
        }
        return status;
    }

    // Ensures that all the caches associated with the given file descriptor's
    // data are flushed all the way to durable media, and can withstand power
    // failures.
    //
    // The path argument is only used to populate the description string in the
    // returned Status if an error occurs.
    static Status SyncFd(int fd, const std::string& fd_path) {
#if HAVE_FULLFSYNC
        // On macOS and iOS, fsync() doesn't guarantee durability past power
    // failures. fcntl(F_FULLFSYNC) is required for that purpose. Some
    // filesystems don't support fcntl(F_FULLFSYNC), and require a fallback to
    // fsync().
    if (::fcntl(fd, F_FULLFSYNC) == 0) {
      return Status::OK();
    }
#endif  // HAVE_FULLFSYNC

#if HAVE_FDATASYNC
        bool sync_success = ::fdatasync(fd) == 0;
#else
        bool sync_success = ::fsync(fd) == 0;
#endif  // HAVE_FDATASYNC

        if (sync_success) {
            return Status::OK();
        }
        return PosixError(fd_path, errno);
    }

    // Returns the directory name in a path pointing to a file.
    //
    // Returns "." if the path does not contain any directory separator.
    static std::string Dirname(const std::string& filename) {
        std::string::size_type separator_pos = filename.rfind('/');
        if (separator_pos == std::string::npos) {
            return std::string(".");
        }
        // The filename component should not contain a path separator. If it does,
        // the splitting was done incorrectly.
        assert(filename.find('/', separator_pos + 1) == std::string::npos);

        return filename.substr(0, separator_pos);
    }

    // Extracts the file name from a path pointing to a file.
    //
    // The returned Slice points to |filename|'s data buffer, so it is only valid
    // while |filename| is alive and unchanged.
    static Slice Basename(const std::string& filename) {
        std::string::size_type separator_pos = filename.rfind('/');
        if (separator_pos == std::string::npos) {
            return Slice(filename);
        }
        // The filename component should not contain a path separator. If it does,
        // the splitting was done incorrectly.
        assert(filename.find('/', separator_pos + 1) == std::string::npos);

        return Slice(filename.data() + separator_pos + 1,
                     filename.length() - separator_pos - 1);
    }

    // True if the given file is a manifest file.
    static bool IsManifest(const std::string& filename) {
        return Basename(filename).starts_with("MANIFEST");
    }

    // buf_[0, pos_ - 1] contains data to be written to fd_.
    char buf_[kWritableFileBufferSize];
    size_t pos_;
    int fd_;
    const bool is_manifest_;  // True if the file's name starts with MANIFEST.
    const std::string filename_;
    const std::string dirname_;  // The directory of filename_.
};

#endif //KVENGINE_ENV_H
