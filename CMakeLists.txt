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

include_directories(${CMAKE_SOURCE_DIR}/include )

add_subdirectory(tests)
add_subdirectory(src)

add_executable(kvengine main.cpp)

target_link_libraries(kvengine source_func)


