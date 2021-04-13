//
// Created by yeye on 2021/4/13.
//

#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "env.h"

Status Env::CreateDir(std::string &dirname) {
    DIR* dir;
    dir = opendir(dirname.c_str());
    //TODO: return false when dir is exist
    if(dir == nullptr)
    {
        std::string path = "./" + dirname;
        mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
        return Status::OK();
    }
    return Status::IOError("Dir exists");
}

Status Env::FileExists(std::string &fname) {
    Status s;
    if(access(fname.c_str(), F_OK) == 0)
        return Status::OK();
    return Status::IOError("FileExits");
}
