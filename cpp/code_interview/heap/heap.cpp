#include "gtest/gtest.h"
#include <vector>
#include <functional>

using namespace std;

template<typename Iter, typename Cmp = less<typename Iter::value_type>>
void heapifyDown(Iter begin, Iter end, Iter cur, const Cmp& cmp = Cmp()) {
    while(cur < end) {
        auto min = cur;
        auto left = begin + (cur - begin) * 2 + 1;
        auto right = left + 1;
        if (left < end && cmp(*left, *min)) {
            min = left;
        }
        if (right < end && cmp(*right, *min)) {
            min = right;
        }
        if (min == cur) {
            break;
        }
        swap(*min, *cur);
        cur = min;
    }
}

template<typename Iter, typename Cmp = less<typename Iter::value_type>>
void makeHeap(Iter begin, Iter end, const Cmp &cmp = Cmp()) {
    auto len = end - begin;
    if (len < 2) {
        return;
    }
    auto cur = begin + len/2 - 1;
    while(true) {
        heapifyDown(begin, end, cur, cmp);
        if (cur == begin) {
            break;
        }
        --cur;
    }
}

template<typename Iter, typename Cmp = less<typename Iter::value_type>>
void pushHeap(Iter begin, Iter end, const Cmp& cmp = Cmp()) {
    auto len = end - begin;
    if (len < 2) {
        return;
    }
    auto cur = end - 1;
    while (true) {
        auto parent = begin + (cur - begin - 1) / 2;
        if (!cmp(*parent, *cur)) {
            swap(*parent, *cur);
            cur = parent;
        } else {
            break;
        }
        if (parent == begin) {
            break;
        }
    }
}

template<typename Iter, typename Cmp = less<typename Iter::value_type>>
void popHeap(Iter begin, Iter end, const Cmp& cmp = Cmp()) {
    swap(*begin, *(--end));
    heapifyDown(begin, end, begin, cmp);
}

int checkHeap(vector<int>& vec, const function<bool (int, int)>& cmp = less<>()) {
    if (vec.size() < 2) {
        return -1;
    }
    auto begin = vec.begin();
    auto end = vec.end();
    auto start = vec.begin() + vec.size()/2 - 1;
    while (true) {
        auto left = begin + (start - begin) * 2 + 1;
        auto right = begin + (start - begin) * 2 + 2;
        if (left < end && cmp(*left, *start)) {
            return start - begin;
        }
        if (right < end && cmp(*right, *start)) {
            return start - begin;
        }
        if (start == begin) {
            break;
        }
        --start;
    }
    return -1;
}

TEST(test, heap) {
    vector<int> hp;
    makeHeap(hp.begin(), hp.end());
    hp = {3, 1};
    makeHeap(hp.begin(), hp.end());
    ASSERT_EQ(hp, vector<int>({1, 3}));
    hp = {9, 8, 7, 6, 5, 4, 3, 2, 1, 1};
    makeHeap(hp.begin(), hp.end());
    ASSERT_EQ(checkHeap(hp), -1);
    ASSERT_EQ(hp[0], 1);
    hp.push_back(0);
    pushHeap(hp.begin(), hp.end());
    ASSERT_EQ(checkHeap(hp), -1);
    ASSERT_EQ(hp[0], 0);
    popHeap(hp.begin(), hp.end());
    ASSERT_EQ(hp[hp.size() - 1], 0);
    hp.pop_back();
    ASSERT_EQ(checkHeap(hp), -1);
    makeHeap(hp.begin(), hp.end(), greater<>());
    ASSERT_EQ(hp[0], 9);
    ASSERT_EQ(checkHeap(hp, greater<>()), -1);
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}