//
// Created by 彭琳峰 on 2020-10-22.
//

#ifndef MYDB_COMPARATOR_H
#define MYDB_COMPARATOR_H


#include <string.h>
#include "../include/index.h"


class Comparator {
public:
    virtual ~Comparator() {}

    virtual int compare(const std::string &a, const std::string &b) const = 0;
};

class StringComparator : public Comparator {
public:
    int compare(const std::string &a, const std::string &b) const {
        return a.compare(b);
    }
};

class Compare {
public:
    Compare(Comparator *comparator)
            : comparator_(comparator) {}

    int operator()(const Index &a, const Index &b) const {
        return comparator_->compare(a.key_, b.key_);
    }

    int operator()(const std::string &a, const std::string &b) const {
        return strcmp(a.c_str(), b.c_str());
    }

private:
    Comparator *comparator_;
};

#endif //MYDB_COMPARATOR_H
