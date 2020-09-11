auto_add_library(heap TYPE INTERFACE)
auto_add_test(test_heap SRCS test/test_heap.cpp DEPENDS heap ${GTEST} utils)