// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_INCLUDE_COMPARATOR_H_
#define STORAGE_LEVELDB_INCLUDE_COMPARATOR_H_

#include <string>


class Slice;


class Comparator {
 public:
  virtual ~Comparator()= default;


  virtual int Compare(const Slice& a, const Slice& b) const = 0;

  virtual const char* Name() const = 0;
  virtual void FindShortestSeparator(std::string* start,
                                     const Slice& limit) const = 0;

  virtual void FindShortSuccessor(std::string* key) const = 0;

};


const Comparator* BytewiseComparator();


#endif  // STORAGE_LEVELDB_INCLUDE_COMPARATOR_H_
