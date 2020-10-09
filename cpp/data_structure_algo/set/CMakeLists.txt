auto_add_library(set TYPE INTERFACE)

auto_add_test(test_set SRCS test/test_set.cpp DEPENDS set ${GTEST})