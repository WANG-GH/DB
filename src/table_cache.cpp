////
//// Created by yeye on 2021/4/17.
////
//
//#include "table_cache.h"
//
//Status TableCache::Get(uint64_t file_number, uint64_t file_size, const Slice &k, void *arg,
//                       void (*handle_result)(void *, const Slice &, const Slice &)) {
//    LRUHandle* handle = nullptr;
////    Status s = FindTable(file_number, file_size,&handle);
////    if(s.ok()){
////
////    }
//}
//
//Status TableCache::FindTable(uint64_t file_number, uint64_t file_size, LRUHandle **handle) {
//    Status s;
//    char buf[sizeof(file_number)];
//    EncodeFixed64(buf, file_number);
//    std::string str_number = std::to_string(file_number);
//    Slice key(str_number);
//    *handle = cache_->Lookup(key);
//
////    if (*handle == nullptr) {
////        std::string fname = TableFileName(dbname_, file_number);
////        RandomAccessFile* file = nullptr;
////        Table* table = nullptr;
////        s = env_->NewRandomAccessFile(fname, &file);
////        if (!s.ok()) {
////            std::string old_fname = SSTTableFileName(dbname_, file_number);
////            if (env_->NewRandomAccessFile(old_fname, &file).ok()) {
////                s = Status::OK();
////            }
////        }
////        if (s.ok()) {
////            s = Table::Open(options_, file, file_size, &table);
////        }
//}
