//
// Created by 彭琳峰 on 2020-10-24.
//

#include "mydb.h"

bool MyDB::Open(const std::string &dbname, MyDB **pdb) {
    *pdb = nullptr;
    //以db指针创建对象，最后再让pdb指向db
    MyDB *db = new MyDB(dbname);
    //创建skiplist tables
    Comparator *comparator = new StringComparator();
    Table *table = new Table(Compare(comparator));
    db->table = table;
    //创建当前dbname的文件夹
    std::string current_dir;
    Env::GetCurrentDir(&current_dir);
    std::string db_dir = current_dir + "/" + dbname;
    if (!Env::FileExists(db_dir)) {
        auto s = Env::CreateDir(db_dir);
        if (!s) {
            return false;
        }
    }
    //打开数据文件
    for (int i = 0; i <5; i++) {
        std::string data_file_name = db_dir + kDataFilePrefix + std::to_string(i);
        PosixFile *file;
        auto s = Env::AddFile(data_file_name, &file);
        if (s) {
            db->data_files_.push_back(file);
        } else {
            return false;
        }
        if (file->GetOffset()!=0){
            db->file_id_ = i;
            db->file_offset_ = file->GetOffset();
        }
    }
    //打开索引文件
    for (int i = 0; i <5; i++) {
        std::string index_file_name = db_dir + kIndexFilePrefix + std::to_string(i);
        PosixFile *file;
        auto s = Env::AddFile(index_file_name, &file);
        if (s) {
            db->index_files_.push_back(file);
        } else {
            return false;
        }
    }


    db->CallBackIndex();
    *pdb = db;
    return true;
}

bool MyDB::Put(const std::string &key, const std::string &value) {
    PosixFile *data_file = data_files_[file_id_];
    //这条记录键为key,在file_id_对应的文件offset开始写,写了data.data_size_个字节
    Data data(key, value);
    int offset = data_file->GetOffset();
    if(likely(offset+data.data_size_<MAX_DATA_SIZE))
    {
        auto s = data_file->AppendData(data);
        if (likely(s)) {
            PosixFile *index_file = index_files_[file_id_];
            Index index(key, file_id_, offset, data.key_size_, data.value_size_);
            s = index_file->AppendIndex(index);
            if (likely(s)) {
                table->Insert(index);
                return true;
            }
        }
    }
    else{
        if(file_id_==5){
            fprintf(stderr,"TOO MUCH DATA!");
            return false;
        }
        data_file = data_files_[++file_id_];
        if(likely(file_offset_+data.data_size_<MAX_DATA_SIZE)){
            auto s = data_file->AppendData(data);
            if (likely(s)) {
                PosixFile *index_file = index_files_[file_id_];
                Index index(key, file_id_, offset, data.key_size_, data.value_size_);
                s = index_file->AppendIndex(index);
                if (likely(s)) {
                    table->Insert(index);
                    return true;
                }
            }
        }
        else{
            fprintf(stderr,"TOO BIG VALUE!\n");
        }
    }


    return false;
}

bool MyDB::Get(const std::string &key, std::string *ret_value) {
    Index search_index(key), result_index;
    //根据key寻找index,并将结果存入result_index
    auto s = table->Get(search_index, &result_index);
    if (likely(s)) {
        //判断是否该记录已经过期,后面用来实现Delete
        if (unlikely(result_index.key_size_ + result_index.value_size_ == 0)) {
            *ret_value = "";
            return false;

        }
        int kv_size = result_index.key_size_ + result_index.value_size_;
        char buf[kv_size];
        auto s = data_files_[result_index.file_id_]->Read(
                buf, result_index.offset_, kv_size);
        if (likely(s)) {
            std::string key; // 传入Decode占位置
            DecodeData(buf, result_index.key_size_,
                       result_index.value_size_, &key, ret_value);
            return true;
        } else {
            return false;
        }

    }
    return false;
}


//从index文件中快速重建目录
bool MyDB::CallBackIndex() {
    /*单线程恢复index,用来debug*/
    for (int i = 0; i < max_files_num_; ++i) {
        PosixFile *file = index_files_[i];
        int file_offset = file->GetOffset();
        int pos = 0;
        while (pos <file_offset) {
            char index_size_buf[sizeof(int)];
            auto s = file->Read(index_size_buf, pos, sizeof(int));  //把index_size读到了buf里
            if (likely(s)) {
                pos += sizeof(int);
                int index_size = DecodeFixedInt(index_size_buf);
                char buf[index_size];
                auto s = file->Read(buf, pos, index_size);
                if (likely(s)) {
                    pos += index_size;
                    Index index;
                    DecodeIndex(buf, &index.key_, &index.file_id_, &index.offset_, &index.key_size_,&index.value_size_);
                    table->Insert(index);
                }

            }
        }

    }

//    /*多线程恢复index*/
//    std::vector<std::thread> threads;
//    for (int i = 0; i < max_files_num_; ++i) {
//        std::thread loader(&MyDB::LoadIndexFromFile, this, index_files_[i]);
//        threads.push_back(std::move(loader)); //将一个左值强制转化为右值引用
//    }
//    for (int i = 0; i < max_files_num_; ++i) {
//        threads[i].join();
//    }
    return true;
}

void MyDB::LoadIndexFromFile(PosixFile *file) {
    int file_offset = file->GetOffset();
    int pos = 0;
    while (pos < file_offset) {
        char index_size_buf[sizeof(int)];
        auto s = file->Read(index_size_buf, pos, sizeof(int));  //把index_size读到了buf里
        if (likely(s)) {
            pos += sizeof(int);
            int index_size = DecodeFixedInt(index_size_buf);
            char buf[index_size];
            auto s = file->Read(buf, pos, index_size);
            if (likely(s)) {
                pos += index_size;
                Index index;
                DecodeIndex(buf, &index.key_, &index.file_id_, &index.offset_, &index.key_size_, &index.value_size_);
                int slot = Hash::HashStr(index.key_.c_str()) ;
                table->Insert(index);
            }

        }
    }
}

void MyDB::SetWritePos(int file_id, int file_offset) {
    file_id_ = file_id;
    file_offset_ = file_offset;

}
