auto_add_library(memory TYPE INTERFACE)
auto_add_test(test_memory SRCS test/test_ptr.cpp DEPENDS memory ${GTEST})