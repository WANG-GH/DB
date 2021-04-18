//
// Created by yeye on 2021/4/13.
//

#include "version_set.h"

void Version::Ref() { ++refs_; }

void Version::Unref() {
    //assert(this != &vset_->dummy_versions_);
    //assert(refs_ >= 1);
    --refs_;
    if (refs_ == 0) {
        delete this;
    }
}

Version::~Version() {

    // Remove from linked list
    prev_->next_ = next_;
    next_->prev_ = prev_;

    // Drop references to files
    for (int level = 0; level < config::kNumLevels; level++) {
        for (size_t i = 0; i < files_[level].size(); i++) {
            FileMetaData* f = files_[level][i];
            f->refs--;
            if (f->refs <= 0) {
                delete f;
            }
        }
    }
}

class VersionSet::Builder{
private:
    struct BySmallestKey {
        const InternalKeyComparator *internal_comparator;

        bool operator()(FileMetaData *f1, FileMetaData *f2) const {
            int r = internal_comparator->Compare(f1->smallest, f2->smallest);
            if (r != 0) {
                return (r < 0);
            } else {
                // Break ties by file number
                return (f1->number < f2->number);
            }
        }
    };
};

Status VersionSet::LogAndApply(VersionEdit *edit) {
    //do log

//    Version* v = new Version(this);
//    {
//        Builder builder(this, current_);
//        builder.Apply(edit);
//        builder.SaveTo(v);
//    }
    return Status();
}

void VersionSet::Finalize(Version *v) {

}
