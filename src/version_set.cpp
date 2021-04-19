//
// Created by yeye on 2021/4/13.
//

#include "version_set.h"

enum SaverState {
    kNotFound,
    kFound,
    kDeleted,
    kCorrupt,
};
struct Saver {
    SaverState state;
    const Comparator* ucmp;
    Slice user_key;
    std::string* value;
};

static void SaveValue(void* arg, const Slice& ikey, const Slice& v)
{
    Saver* saver = reinterpret_cast<Saver*>(arg);
    //TODO: finish the ParsedKey
}

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

void
Version::ForEachOverlapping(Slice user_key, Slice internal_key, void *arg,
                            bool (*func)(void *, int, FileMetaData *)) {
    Comparator* ucmp = new InternalKeyComparator;

    //only imp the level 0;
    std::vector<FileMetaData* > tmp;
    for(int i = 0; i < files_[0].size(); i++)
    {
        FileMetaData* f = files_[0][i];
        if(ucmp->Compare(user_key, f->smallest.user_key) >= 0
            && ucmp->Compare(user_key, f->largest.user_key) <= 0)
            tmp.push_back(f);
    }

    if(!tmp.empty())
        std::sort(tmp.begin(), tmp.end(),[] (FileMetaData* a, FileMetaData* b)-> bool{
                return a->number > b->number;
            });

    //why 0????
    for(int i = 0; i < tmp.size(); i++){
        if( !(*func)(arg, 0, tmp[i]) ){
            return;
        }
    }
}

Status Version::Get(const LookupKey &key, std::string *val, Version::GetStats *stats) {
//    struct State{
//        Saver saver;
//        GetStats* stats;
//        FileMetaData* last_find_file;
//        int last_file_read_level;
//        bool find;
//        Slice ikey;
//        Status s;
//        VersionSet* vset;
//
//        bool Match(void* arg, int n, FileMetaData* f)
//        {
//            State* state = reinterpret_cast<State*>(arg);
//
//            //we have more than one file?
//            if(state->stats->seek_file == nullptr
//                && state->last_find_file != nullptr){
//                state->stats->seek_file = state->last_find_file;
//                state->stats->seek_file_level = state->last_file_read_level;
//            }
//
//            state->last_file_read_level = n;
//            state->s = state->vset->table_->Get(f->number, f->file_size, state->ikey, &state->saver, SaveValue);
//
//
//        }
//    };
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
