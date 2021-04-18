//
// Created by yeye on 2021/4/13.
//

#ifndef KVENGINE_VERSION_SET_H
#define KVENGINE_VERSION_SET_H

#include <set>
#include <vector>
#include "dbformat.h"
#include "version_edit.h"
#include "status.h"

class Version{
public:
    // Lookup the value for key.  If found, store it in *val and
    // return OK.  Else return a non-OK status.  Fills *stats.
    //TODO: find out what is it
    struct GetStats {
        FileMetaData* seek_file;
        int seek_file_level;
    };

    //Status Get(const LookupKey& key, std::string* val,
    //           GetStats* stats);

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
};

class VersionSet{
public:
    Status LogAndApply(VersionEdit* edit);
    Status Recover(bool* save_manifest);

    //Add all files listed in any live version to *live.
    //uni64_t file number
    void AddLiveFiles(std::set<uint64_t>* live);
private:
    class Builder;
    void Finalize(Version* v);
};


#endif //KVENGINE_VERSION_SET_H
