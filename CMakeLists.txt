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

set(SRC "./src/arena.cpp" "./src/memtable.cpp")
include_directories(${CMAKE_SOURCE_DIR}/include )

add_executable(kvengine main.cpp ${SRC}  )

add_subdirectory(tests) 