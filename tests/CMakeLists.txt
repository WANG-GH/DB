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
aux_source_directory(. SRC)

add_executable(run_actual_tests  test.cpp "block_test.cpp" "sstable_test.cpp" "mem_table_test.cpp" )

target_link_libraries(run_actual_tests gtest gtest_main source_func )
