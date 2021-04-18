##
# @file CMakeLists.txt
# @brief
# tests目录下cmake文件
#
# @author linfp
# @email linfp123@126.com
#
add_subdirectory(googletest)
include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)
set(SRC "../src/memtable.cpp" "../src/arena.cpp" "../src/env.cpp" "../src/status.cpp")


add_executable(run_actual_tests  test.cpp  "./mem_table_test.cpp" "./cache_test.cpp" "./env_test.cpp" ${SRC})
target_link_libraries(run_actual_tests gtest gtest_main)
