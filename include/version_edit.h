//
// Created by yeye on 2021/4/13.
//

#ifndef KVENGINE_VERSION_EDIT_H
#define KVENGINE_VERSION_EDIT_H
//#include <utility>
#include <vector>
#include "dbformat.h"
#include "status.h"
#include <set>
class VersionSet;
class Version;
//recode the file data
struct FileMetaData{
    FileMetaData() : refs(0), allowed_seeks(1 << 30), file_size(0) {}

    int refs;
    int allowed_seeks;  // Seeks allowed until compaction
    uint64_t number;
    uint64_t file_size;    // File size in bytes
    InternalKey smallest;  // Smallest internal key served by table
    InternalKey largest;   // Largest internal key served by table

};

class VersionEdit{
public:
    void AddFile(int level, uint64_t file, uint64_t file_size,
                 const InternalKey& smallest, const InternalKey& largest);

    // REQUIRES: This version has not been saved (see VersionSet::SaveTo)
    void RemoveFile(int level, uint64_t file) ;

    //for manifest
    void EncodeTo(std::string* dst) const;
    Status DecodeFrom(const Slice& src);

private:
    friend class VersionSet;

    typedef std::set<std::pair<int, uint64_t> > DeleteFileSet;

    std::string comparator_;
    uint64_t nex_file_number_;
    SequenceNumber last_sequence_;
    bool has_comparator_;
    bool has_next_file_number_;
    bool has_last_sequence_;

    DeleteFileSet deleted_files_;
    std::vector<std::pair<int, FileMetaData>> new_files_;
};


#endif //KVENGINE_VERSION_EDIT_H
