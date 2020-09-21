auto_add_library(queue TYPE INTERFACE DEPENDS vector)

auto_add_test(test_queue SRCS test/test_queue.cpp DEPENDS queue ${GTEST})