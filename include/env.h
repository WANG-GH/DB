//
// Created by yeye on 2021/4/13.
//

#ifndef KVENGINE_ENV_H
#define KVENGINE_ENV_H

#include <string>

#include "status.h"
class Env{
public:
    Status CreateDir(std::string& dirname);
    Status FileExists(std::string& fname);
};
#endif //KVENGINE_ENV_H
