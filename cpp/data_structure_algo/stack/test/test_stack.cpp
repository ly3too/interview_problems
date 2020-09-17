#include "gtest/gtest.h"
#include "stack/stack.hpp"
#include <stack>

using namespace std;

TEST(test, stack) {
    stack<int> s1;
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
