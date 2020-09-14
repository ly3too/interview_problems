auto_add_library(list TYPE INTERFACE)
auto_add_test(test_list SRCS test/test_list.cpp DEPENDS list ${GTEST})