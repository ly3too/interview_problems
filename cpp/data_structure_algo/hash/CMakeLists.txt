auto_add_library(hash TYPE INTERFACE DEPENDS list vector)

auto_add_test(test_hash SRCS test/test_hash.cpp DEPENDS hash ${GTEST})