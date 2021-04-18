//
// Created by yeye on 2021/4/13.
//

#include "YeapDB.h"

Status YeapDB::Open(const Options &opt, const std::string &name) {

    VersionEdit edit;
    bool save_manifest = false;
    Status s = Recover(&edit, &save_manifest);

    return Status();
}

Status YeapDB::Recover(VersionEdit *edit, bool *save_manifest) {

    return Status();
}
