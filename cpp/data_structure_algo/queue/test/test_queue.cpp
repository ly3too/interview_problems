#include "queue/queue.hpp"
#include "gtest/gtest.h"
#include <deque>
#include <queue>

using namespace std;

bool operator==(const Deque<int, allocator<int>, 5> &q1, const deque<int> &q2) {
    auto it1 = q1.begin();
    for (auto &item: q2) {
        EXPECT_EQ(*it1, item);
        if (*it1 != item) {
            return false;
        }
        ++it1;
    }
    return true;
}

TEST(test, queue) {
    Deque<int, allocator<int>, 5> dq1;
    deque<int> dq2;
    ASSERT_TRUE(dq1.empty());

    for (auto i = 0; i < 15; ++i) {
        dq1.emplace_back(i);
        dq2.emplace_back(i);
    }
    ASSERT_EQ(dq1, dq2);
    ASSERT_EQ(dq1[0], 0);
    ASSERT_EQ(dq1[14], 14);
    dq1.pop_front();
    dq2.pop_front();
    ASSERT_EQ(dq1[0], 1);
    ASSERT_EQ(dq1[13], 14);
    dq1.pop_back();
    dq2.pop_back();

    for (int i = 0; i < 3; ++i) {
        dq1.emplace_front(i);
        dq2.emplace_front(i);
    }
    ASSERT_EQ(dq1, dq2);

    auto dq3 = dq1;
    ASSERT_EQ(dq3[0], dq1[0]);
    ASSERT_EQ(dq3.size(), dq1.size());
    ASSERT_EQ(dq3.back(), dq1.back());
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}

