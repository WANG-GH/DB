//
// Created by Linfp on 2021/4/13.
//

#ifndef KVENGINE_ITERATOR_H
#define KVENGINE_ITERATOR_H
#include "slice.h"

class Iterator {
public:
        Iterator(){}

        Iterator(const Iterator&) = delete;
        Iterator& operator=(const Iterator&) = delete;

        virtual ~Iterator(){}

        virtual bool Valid() const = 0;

        virtual void SeekToFirst() = 0;

        virtual void SeekToLast() = 0;

        virtual void Seek(const Slice& target) = 0;

        virtual void Next() = 0;

        virtual void Prev() = 0;

    virtual Slice key() const = 0;

    virtual Slice value() const = 0;

};




#endif //KVENGINE_ITERATOR_H
