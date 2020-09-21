#include "vector/vector.hpp"
#include "gtest/gtest.h"
#include <vector>

using namespace std;

template<typename T>
bool operator==(const vector<T> &vec1, const Vector<T> &vec2) {
    if (vec1.size() != vec2.size()) {
        return false;
    }

    auto it1 = vec1.begin();
    for (auto it2 = vec2.begin(); it2 != vec2.end(); ++it2, ++it1) {
        EXPECT_EQ(*it1, *it2);
        if (*it1 != *it2) {
            return false;
        }
    }
    return true;
}

TEST(test, vector) {
    vector<string> vec1(8);
    Vector<string> vec2(8);
    fill(vec1.begin() + 1, vec1.end(), "123");
    fill(vec2.begin() + 1, vec2.end(), "123");
    ASSERT_EQ(vec1, vec2);
    ASSERT_EQ(vec2.capacity(), 8);
    ASSERT_EQ(vec1[0], vec2[0]);
    ASSERT_EQ(vec1[7], "123");

    // grow
    vec1.emplace_back("2");
    vec2.emplace_back("2");
    ASSERT_EQ(vec1, vec2);
    ASSERT_EQ(vec2.capacity(), 16);

    // shrink
    for (auto i = 0; i < 6; ++i) {
        vec1.pop_back();
        vec2.pop_back();
        ASSERT_EQ(vec1.back(), vec2.back());
    }
    ASSERT_EQ(vec1, vec2);
    ASSERT_EQ(vec2.capacity(), 8);
}

TEST(test, dqvector) {
    {
        DqVector<int> vec;
        DqVector<int> vec2(3, 11);
        ASSERT_EQ(vec.capacity(), 0);
        ASSERT_EQ(vec2.capacity(), 3);
        ASSERT_EQ(vec2[1], 11);
        for (auto &item: vec2) {
            ASSERT_EQ(item, 11);
        }
    }
    DqVector<int, allocator<int>, true, true, 5> vec;
    vec.emplace_back(1);
    ASSERT_EQ(vec.size(), 1);
    ASSERT_EQ(vec.capacity(), 5);
    vec.emplace_front(-1);
    ASSERT_EQ(vec.size(), 2);
    ASSERT_EQ(vec.capacity(), 10);
    ASSERT_EQ(vec[0], -1);
    ASSERT_EQ(vec[1], 1);
    ASSERT_EQ(vec.back(), 1);

    // grow
    for (auto i = 2; i <= 6; ++i) {
        vec.emplace_back(i);
    }
    // when add last one grow size of;
    ASSERT_EQ(vec.capacity(), 16);
    ASSERT_EQ(vec.front_space(), 4);
    for (auto i = 2; i <= 6; ++i) {
        vec.emplace_front(-i);
    }
    // when add last one grow size of 11
    ASSERT_EQ(vec.capacity(), 27);
    ASSERT_EQ(vec.size(), 12);
    ASSERT_EQ(vec[0], -6);
    ASSERT_EQ(vec[11], 6);

    // now left space 10, right space 5
    // shrink
    for (auto i = 0; i < 7; ++i) {
        vec.pop_front();
    }
    ASSERT_EQ(vec.front_space(), 5);
    for (auto i = 0; i < 3; ++i) {
        vec.pop_back();
    }
    ASSERT_EQ(vec.back_space(), 5);
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
