auto_add_library(stack TYPE INTERFACE DEPENDS list queue)
auto_add_test(test_stack SRCS test/test_stack.cpp DEPENDS stack ${GTEST})