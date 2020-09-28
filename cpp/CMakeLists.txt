cmake_minimum_required(VERSION 3.15)
project(problems)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmakefiles ${CMAKE_MODULE_PATH})
include(util)

find_package(GTest REQUIRED PATHS ext/${CMAKE_SYSTEM_NAME}/gtest NO_DEFAULT_PATH)
set(GTEST GTest::gtest GTest::gtest_main)

include (CTest) # for memcheck
find_program(MEMORYCHECK_COMMAND valgrind)
set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=yes --show-reachable=yes --num-callers=50 --track-fds=yes --trace-children=yes")

enable_testing()
auto_add_subdirs()