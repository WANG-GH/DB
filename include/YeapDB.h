//
// Created by yeye on 2021/4/13.
//

#ifndef KVENGINE_YEAPDB_H
#define KVENGINE_YEAPDB_H
#include <string>
#include "status.h"
#include "version_set.h"
#include "memtable.h"
#include "options.h"
#include "table_cache.h"
#include "version_edit.h"
//no snapshot and iterator
class YeapDB{
public:
    Status Open(const Options& opt, const std::string& name);
    Status Put(const Slice& key, const Slice& value);
    Status Get(const Slice& key, std::string* value);
    Status Delete(const Slice& key);
private:
  //  Status Recover(VersionEdit* edit, bool* save_manifest);
    const Options options_;
    const std::string dbname_;
  //  TableCache* table_cache_;
    MemTable* mem_;
    MemTable* imm_;
};

#endif //KVENGINE_YEAPDB_H
