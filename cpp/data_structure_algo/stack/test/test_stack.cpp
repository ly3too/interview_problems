#include "gtest/gtest.h"
#include "stack/stack.hpp"
#include <stack>

using namespace std;

TEST(test, stack) {
    {
        Stack<int> s1;
        ASSERT_TRUE(s1.empty());
        s1.emplace(1);
        s1.emplace(2);
        s1.emplace(3);
        ASSERT_EQ(s1.size(), 3);
        ASSERT_EQ(s1.top(), 3);
        s1.pop();
        ASSERT_EQ(s1.top(), 2);
        s1.pop();
        ASSERT_EQ(s1.top(), 1);
        ASSERT_FALSE(s1.empty());
        s1.pop();
        ASSERT_TRUE(s1.empty());
    }
    {
        Stack<int, allocator<int>, List<int>> s1(2, 2);
        ASSERT_EQ(s1.size(), 2);
        s1.emplace(3);
        ASSERT_EQ(s1.size(), 3);
        ASSERT_EQ(s1.top(), 3);
        s1.pop();
        ASSERT_EQ(s1.top(), 2);
        s1.pop();
        ASSERT_EQ(s1.top(), 2);
        ASSERT_FALSE(s1.empty());
        s1.pop();
        ASSERT_TRUE(s1.empty());
    }
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
