////
//// Created by yeye on 2021/4/19.
////
//
//#include "version_edit.h"
//
//void VersionEdit::AddFile(int level, uint64_t file, uint64_t file_size, const InternalKey &smallest,
//                          const InternalKey &largest) {
//    {
//        FileMetaData f;
//        f.number = file;
//        f.file_size = file_size;
//        f.smallest = smallest;
//        f.largest = largest;
//        new_files_.push_back(std::make_pair(level, f));
//    }
//}
//
//void VersionEdit::RemoveFile(int level, uint64_t file) {
//    deleted_files_.insert(std::make_pair(level, file));
//}
