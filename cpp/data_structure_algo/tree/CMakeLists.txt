auto_add_library(tree TYPE INTERFACE DEPENDS stack)

auto_add_test(test_tree SRCS test/test_tree.cpp test/test_bst.cpp test/test_avl_rbtree.cpp DEPENDS tree ${GTEST})