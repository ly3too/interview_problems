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

// 考虑溢出，符号，空格，base
int stoi1(const string &s, size_t* pos = nullptr, int base = 10) {
    int ret = 0;
    auto it = s.begin();
    while (it != s.end() && isspace(*it)) {
        ++it;
    }

    // 符号
    bool negative = false;
    if (it != s.end() && (*it == '+' || *it == '-')) {
        if (*it == '-') {
            negative = true;
        }
        ++it;
    }

    // 基数
    auto start_it = it;
    if (base == 0) {
        base = 10;
        if ((it + 1) != s.end() && *it == '0') {
            if (tolower(*(it + 1)) == 'x') {
                base = 16;
                ++it;
            } else if (isdigit(*(it + 1))) {
                base = 8;
            }
            ++it;
        }
    }

    // 构造溢出值
    int overTh;
    if (negative) {
        overTh = -(numeric_limits<int>::min() / base);
    } else {
        overTh = numeric_limits<int>::max() / base;
    }
    int overLastDigit;
    if (negative) {
        overLastDigit = -(numeric_limits<int>::min() % base);
    } else {
        overLastDigit = numeric_limits<int>::max() % base;
    }
    if (overLastDigit <= 0) {
        overLastDigit += base;
    }

    while(it != s.end()) {
        int val;
        auto ch = *it;
        if (isdigit(ch)) {
            val = ch - '0';
        } else if (isalpha(ch)) {
            val = tolower(ch) - 'a' + 10;
        } else {
            break;
        }

        if (val >= base) {
            break;
        }

        if (ret > overTh || ret == overTh && val > overLastDigit) {
            throw overflow_error(s);
        }
        ret = ret * base + val;
        ++it;
    }

    if (start_it == it) {
        throw invalid_argument(s);
    }

    if (negative) {
        ret = 0 - ret;
    }
    if (pos) {
        *pos = it - s.begin();
    }
    return static_cast<int>(ret);
}

int stoi2(const string& s, size_t *end_pos = nullptr, int base = 10) {
    auto it = s.begin();
    auto end = s.end();
    auto begin = s.begin();

    // skip spacke
    while (it < end && isspace(*it)) {
        ++it;
    }

    // sign
    auto positive = true;
    if (it < end) {
        if (*it == '-') {
            positive = false;
            ++it;
        } else if (*it == '+') {
            ++it;
        }
    }

    // find base
    auto start_it = it;
    if (base == 0) {
        base = 10;
        if (it + 1 < end && *it == '0') {
            if (tolower(*(it + 1)) == 'x') {
                base = 16;
                ++it;
                ++it;
            } else if (isdigit(*(it + 1))) {
                base = 8;
                ++it;
            }
        }
    }

    // find limit
    int overTh;
    int lastDigitTh;
    if (positive) {
        overTh = numeric_limits<int>::max() / base;
        lastDigitTh = numeric_limits<int>::max() % base;
    } else {
        overTh = - (numeric_limits<int>::min() / base);
        lastDigitTh = - (numeric_limits<int>::min() % base);
    }
    if (lastDigitTh == 0) {
        lastDigitTh += base;
    }

    int ret = 0;
    while(it < end) {
        auto ch = *it;
        int val = 0;
        if (isdigit(ch)) {
            val = static_cast<int>(ch - '0');
        } else if (isalpha(ch)) {
            val = static_cast<int>(tolower(ch) - 'a' + 10);
        } else {
            break;
        }
        if (val >= base) {
            break;
        }

        // check overflow
        if (ret > overTh || ret == overTh && val > lastDigitTh) {
            throw overflow_error("stoi");
        }
        ret = ret * base + val;
        ++it;
    }

    if (it == start_it) {
        throw invalid_argument("stoi");
    }

    if (!positive) {
        ret = 0 - ret;
    }
    if (end_pos) {
        *end_pos = it - begin;
    }
    return ret;
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

TEST(test, stoi) {
    ASSERT_EQ(stoi2(" +08", nullptr, 0), 0);
    ASSERT_EQ(stoi2("1234"), 1234);
    ASSERT_EQ(stoi2("    +123"), stoi("   +123"));
    ASSERT_EQ(stoi2("    -234"), -234);
    ASSERT_EQ(stoi2(to_string(numeric_limits<int>::min())), numeric_limits<int>::min());
    ASSERT_EQ(stoi2(to_string(numeric_limits<int>::max())), numeric_limits<int>::max());
    try {
        stoi2(to_string((long long)(numeric_limits<int>::min()) - 1));
        ASSERT_FALSE(true);
    } catch (overflow_error& e) {
    }

    try {
        stoi2(to_string((long long)(numeric_limits<int>::max()) + 1));
        ASSERT_FALSE(true);
    } catch (overflow_error& e) {
    }

    ASSERT_EQ(stoi2("0xpp", nullptr, 0), stoi("0xpp", nullptr, 0));

    try {
        stoi2("");
        FAIL() << "should throw";
    } catch (invalid_argument& e) {
    }

    try {
        stoi2("  +");
        FAIL() << "should throw";
    } catch (invalid_argument& e) {
    }

    size_t pos1 = 0;
    size_t pos2 = 0;
    string str = "    0x123efgg";
    ASSERT_EQ(stoi2(str, &pos1, 0), stoi(str, &pos2, 0));
    ASSERT_EQ(pos1, pos2);
    ASSERT_EQ(stoi2("    0123", nullptr, 0), 0123);
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}