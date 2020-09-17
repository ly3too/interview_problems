#include "queue/queue.hpp"
#include "gtest/gtest.h"
#include <deque>
#include <queue>

using namespace std;

TEST(test, queue) {
    deque<int> dq;
    dq.emplace_back();
    queue<int> que;
    que.emplace();
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}

