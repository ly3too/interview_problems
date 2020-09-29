#include "hash/hash.hpp"
#include "gtest/gtest.h"
#include <functional>
#include <unordered_map>

using namespace std;
using namespace my_hash;

enum class Enu1 {
    DEFAULT = 0,
};

TEST(test, hash) {
    auto val1 = 1;
    auto val2 = 2;
    cout << "MyHash(val1) = " << MyHash(val1) << endl;
    ASSERT_EQ(MyHash(val1), std::size_t(val1));
    ASSERT_EQ(MyHash(val2), std::size_t(val2));
    ASSERT_EQ(MyHash(&val1), std::size_t(&val1));

    Enu1 enu = Enu1::DEFAULT;
    ASSERT_EQ(MyHash(enu), std::size_t(enu));

    string str1 = "abc";
    string str2 = str1;
    cout << "MyHash(str1) = " << MyHash(str1) << endl;
    ASSERT_EQ(MyHash(str1), MyHash(str2));
    ASSERT_EQ(MyHash(str1.c_str(), str1.size()), MyHash(str2));

    cout << "MyHash<DJB2Hash>(str1) = " << MyHash<string, DJB2Hash>(str1) << endl;
    ASSERT_EQ((MyHash<string, DJB2Hash>(str1)), (MyHash<string, DJB2Hash>(str2)));

    cout << "MyHash(\"abcd\", 4) = " << MyHash("abcd", 4) << endl;
    auto vec = vector<int>(1);
    cout << "MyHash(vector<int>(1)) = " << MyHash(vec) << endl;
    vec.emplace_back(1);
    cout << "MyHash(vector<int>(1)) = " << MyHash(vec) << endl;
}

TEST(test, hash_map) {
    FixedHashMap<string, string> map(100);
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}