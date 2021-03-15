//
// Created by 彭琳峰 on 2020-10-25.
//

#ifndef MYDB_HASH_H
#define MYDB_HASH_H

class Hash {
public:
    static unsigned int HashStr(const char *str) {
        unsigned int seed = 131;
        unsigned int hash = 0;

        while (*str) {
            hash = hash * seed + (*str++);
        }
        return (hash & 0x7FFFFFFF);
    }
};

#endif //MYDB_HASH_H
