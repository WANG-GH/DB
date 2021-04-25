//
// Created by Linfp on 2021/4/24.
//

#include "dbformat.h"

void AppendInternalKey(std::string *result, const ParsedInternalKey &key) {
    result->append(key.user_key.data(), key.user_key.size());
    PutFixed64(result, PackSequenceAndType(key.sequence, key.type));
}
