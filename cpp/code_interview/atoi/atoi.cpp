#include "gtest/gtest.h"
#include <cctype>
#include <limits>

using namespace std;

enum class MyAtoiErr {
    OK = 0,
    NULL_POINTER,
    OVER_FLOW,
    INVALID_CHAR,
};

int my_atoi(const char *str, MyAtoiErr &err) {
    err = MyAtoiErr::OK;
    if (!str) {
        err = MyAtoiErr::NULL_POINTER;
        return 0;
    }

    // skip empty
    while (isspace(*str)) {
        ++str;
    }

    int sign = 1; // -1 for negative
    //check sign
    if ('-' == *str) {
        sign = -1;
        ++str;
    } else if ('+' == *str) {
        ++str;
    }

    const static auto max = numeric_limits<int>::max();
    const static auto min = numeric_limits<int>::min();
    const static long long abs_min = -static_cast<long long>(min);

    long long val = 0;
    while (*str) {
        if (isdigit(*str)) {
            int cur_digit = (*str - '0');
            val = val * 10 + cur_digit;

            // check overflow
            if ((val > max && sign > 1) || val > abs_min) {
                err = MyAtoiErr::OVER_FLOW;
                if (sign > 0) {
                    val = max;
                } else {
                    val = min;
                }
                break;
            }
        } else {
            err = MyAtoiErr::INVALID_CHAR;
            break;
        }

        ++str;
    }

    return static_cast<int>(val * sign);
}

TEST(test, atoi) {
    MyAtoiErr err;
    ASSERT_EQ(my_atoi(" 1s", err), 1);
    ASSERT_EQ(err, MyAtoiErr::INVALID_CHAR);

    // valid num
    ASSERT_EQ(my_atoi("123", err), 123);
    ASSERT_EQ(err, MyAtoiErr::OK);
    // with sign
    ASSERT_EQ(my_atoi("+123", err), 123);
    ASSERT_EQ(err, MyAtoiErr::OK);
    ASSERT_EQ(my_atoi("-123", err), -123);
    ASSERT_EQ(err, MyAtoiErr::OK);
    // with space
    ASSERT_EQ(my_atoi(" \n \t -123", err), -123);
    ASSERT_EQ(err, MyAtoiErr::OK);

    auto max = numeric_limits<int>::max();
    auto min = numeric_limits<int>::min();

    // overflow
    ASSERT_EQ(my_atoi("99999999999999999999999999999", err), max);
    ASSERT_EQ(err, MyAtoiErr::OVER_FLOW);
    ASSERT_EQ(my_atoi("-9999999999999999999999999999", err), min);
    ASSERT_EQ(err, MyAtoiErr::OVER_FLOW);
    ASSERT_EQ(my_atoi(to_string(max).c_str(), err), max);
    ASSERT_EQ(err, MyAtoiErr::OK);
    ASSERT_EQ(my_atoi(to_string(min).c_str(), err), min);
    ASSERT_EQ(err, MyAtoiErr::OK);
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}