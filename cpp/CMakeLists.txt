cmake_minimum_required(VERSION 3.15)
project(problems)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmakefiles ${CMAKE_MODULE_PATH})
include(util)

find_package(GTest REQUIRED PATHS ext/${CMAKE_SYSTEM_NAME}/gtest NO_DEFAULT_PATH)
set(GTEST GTest::gtest GTest::gtest_main)

if (UNIX)
    include (CTest) # for memcheck
    # run with command
    # ctest --overwrite MemoryCheckCommandOptions="--leak-check=full --show-leak-kinds=all --trace-children=yes" \
    #      --overwrite MemoryCheckSuppressionFile=/path/to/valgrind.suppressions \
    #      -T memcheck
endif()

enable_testing()
auto_add_subdirs()