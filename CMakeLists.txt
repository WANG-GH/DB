cmake_minimum_required(VERSION 3.16)
project(MyDB)

set(CMAKE_CXX_STANDARD 14)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
add_executable(MyDB main.cpp
        util/skiplist.h
        util/random.h
        util/coding.h
        include/comparator.h
        include/env.h
        include/index.h
        include/data.h
        file/file.h db/mydb.cpp db/mydb.h util/coding.cpp include/atomic_pointer.h util/arena.cpp util/arena.h util/hash.h util/spinlock.h)