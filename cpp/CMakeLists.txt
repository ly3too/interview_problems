cmake_minimum_required(VERSION 3.15)
project(problems)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmakefiles ${CMAKE_MODULE_PATH})
include(util)

if (WIN32)
    find_package(GTest REQUIRED PATHS ext/gtest)
    set(GTEST GTest::gtest GTest::gtest_main)
else()
    find_package(GTest REQUIRED)
    set(GTEST GTest::GTest GTest::Main)
endif()

enable_testing()
auto_add_subdirs()