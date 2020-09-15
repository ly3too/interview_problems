auto_add_library(vector TYPE INTERFACE)
auto_add_test(test_vector SRCS test/test_vector.cpp DEPENDS ${GTEST} vector)