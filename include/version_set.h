//
// Created by yeye on 2021/4/13.
//

#ifndef KVENGINE_VERSION_SET_H
#define KVENGINE_VERSION_SET_H

#include <set>
#include <vector>
#include <algorithm>
#include "dbformat.h"
#include "version_edit.h"
#include "status.h"
#include "table_cache.h"
class Version{
public:
    // Lookup the value for key.  If found, store it in *val and
    // return OK.  Else return a non-OK status.  Fills *stats.
    //TODO: find out what is it
    struct GetStats {
        FileMetaData* seek_file;
        int seek_file_level;
    };

    Status Get(const LookupKey& key, std::string* val,
               GetStats* stats);

    void Ref();
    void Unref();
private:
    friend class VersionSet;

    explicit Version(VersionSet* vset)
            : vset_(vset),
              next_(this),
              prev_(this),
              refs_(0)
              {};
    ~Version();
    //not allow copy
    Version(const Version&) = delete;
    Version& operator=(const Version&) = delete;

    VersionSet* vset_;  // VersionSet to which this Version belongs
    Version* next_;     // Next version in linked list
    Version* prev_;     // Previous version in linked list
    int refs_;          // Number of live refs to this version

    std::vector<FileMetaData*> files_[config::kNumLevels];

    void ForEachOverlapping(Slice user_key, Slice internal_key, void* arg,
                            bool (*func)(void*, int, FileMetaData*));
};

class VersionSet{
public:
    Status LogAndApply(VersionEdit* edit);
    Status Recover(bool* save_manifest);

    //Add all files listed in any live version to *live.
    //uni64_t file number
    void AddLiveFiles(std::set<uint64_t>* live);
private:
    friend class Version;
    class Builder;
    void Finalize(Version* v);
    TableCache* table_;
};


#endif //KVENGINE_VERSION_SET_H
