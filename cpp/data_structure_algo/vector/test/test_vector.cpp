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
    fill(vec1.begin(), vec1.end(), "123");
    fill(vec2.begin(), vec2.end(), "123");
    ASSERT_EQ(vec1, vec2);
    ASSERT_EQ(vec2.capacity(), 8);

    // resize
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

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
