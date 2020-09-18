/**
 * 请实现一个函数用来匹配包括'.'和'*'的正则表达式。模式中的字符'.'表示任意一个字符，
 * 而'*'表示它前面的字符可以出现任意次（包含0次）。 在本题中，匹配是指字符串的所有字符匹配整个模式。
 * 例如，字符串"aaa"与模式"a.a"和"ab*ac*a"匹配，但是与"aa.a"和"ab*a"均不匹配
 */

#include "gtest/gtest.h"
#include <cstring>
#include <vector>
#include <algorithm>

using namespace std;

class Solution {
public:
    bool match(const char* str, const char* pattern) {
        if (!str || !pattern) {
            return false;
        }
        auto len_s = strlen(str);
        auto len_pat = strlen(pattern);

        vector<vector<bool>> dp(len_s + 1); // dp[i[j] means str[0:i] matches pattern[0:j]
        fill(dp.begin(), dp.end(), vector<bool>(len_pat + 1));
        dp[0][0] = true;
        for (size_t j = 2; j <= len_pat; ++j) {
            if (pattern[j - 1] == '*') {
                dp[0][j] = dp[0][j - 2];
            }
        }
        for (size_t i = 1; i <= len_s; ++i) {
            for (size_t j = 1; j <= len_pat; ++j) {
                if (str[i - 1] == pattern[j - 1] || '.' == pattern[j - 1]) {
                    dp[i][j] = dp[i][j] || dp[i - 1][j - 1];
                }
                if (pattern[j - 1] == '*' && j >= 2) {
                    if (pattern[j - 2] == str[i - 1] || '.' == pattern[j - 2]) {
                        dp[i][j] = dp[i][j] || dp[i][j - 2]; // 'a*' as empty
                        dp[i][j] = dp[i][j] || dp[i - 1][j]; // a* as at lest 2 char
                        dp[i][j] = dp[i][j] || dp[i][j-1]; // a* as 1 char
                    } else {
                        dp[i][j] = dp[i][j] || dp[i][j - 2]; // a* as 0 char
                    }
                }
            }
        }
        return dp[len_s][len_pat];
    }
};

TEST(test, match) {
    Solution so;
    ASSERT_TRUE(so.match("", ""));
    ASSERT_FALSE(so.match("a", "b"));
    ASSERT_TRUE(so.match("a", "ab*"));
    ASSERT_TRUE(so.match("aabbbbbb", "aab*"));
    ASSERT_TRUE(so.match("aabbbbbb", "aa.*"));
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}

