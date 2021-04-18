##
# @file CMakeLists.txt
# @brief
# 根目录下cmake文件
#
# @author linfp
# @email linfp123@126.com
#
cmake_minimum_required(VERSION 3.13)
project(kvengine)

set(CMAKE_CXX_STANDARD 14)

set(SRC "./src/arena.cpp" "./src/memtable.cpp" include/YeapDB.h include/options.h include/table_cache.h include/version_set.h src/YeapDB.cpp include/version_edit.h include/env.h src/env.cpp src/status.cpp include/dbformat.h src/version_set.cpp src/cache.cpp include/cache.h src/table_cache.cpp include/table.h src/table.cpp)
include_directories(${CMAKE_SOURCE_DIR}/include )

add_executable(kvengine main.cpp ${SRC}  )

add_subdirectory(tests) 