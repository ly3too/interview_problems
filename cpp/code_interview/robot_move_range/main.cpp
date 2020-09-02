/**
 * https://www.nowcoder.com/practice/6e5207314b5241fb83f2329e89fdecc8?tpId=13&tqId=11219&tPage=1&rp=1&ru=/ta/coding-interviews&qru=/ta/coding-interviews/question-ranking&from=cyc_github
 * 地上有一个m行和n列的方格。一个机器人从坐标0,0的格子开始移动，每一次只能向左，右，上，下四个方向移动一格，
 * 但是不能进入行坐标和列坐标的数位之和大于k的格子。 例如，当k为18时，机器人能够进入方格（35,37），
 * 因为3+5+3+7 = 18。但是，它不能进入方格（35,38），因为3+5+3+8 = 19。请问该机器人能够达到多少个格子？
 */

#include "gtest/gtest.h"
#include <vector>
#include <algorithm>

using namespace std;

class Solution {
public:
    int movingCount(int threshold, int rows, int cols) {
        m_th = threshold;
        init_sum(max(rows, cols));
        m_count = 0;
        m_mask.resize(rows);
        m_rows = rows;
        m_cols = cols;        fill(m_mask.begin(), m_mask.end(), vector<bool>(cols, false));
        try_move(0, 0);
        return m_count;
    }

private:

    void try_move(int r, int c) {
        // 不合法点
        if (r < 0 || r >= m_rows || c < 0 || c >= m_cols
            || m_mask[r][c]) {
            return;
        }
        if (m_sum[r] + m_sum[c] > m_th) {
            m_mask[r][c] = true;
            return;
        }

        // 合法点
        ++m_count;
        m_mask[r][c] = true;
        try_move(r + 1, c);
        try_move(r - 1, c);
        try_move(r, c + 1);
        try_move(r, c - 1);
    }

    /**
     * 计算每个数的和
     * @param max
     */
    void init_sum(int max) {
        m_sum.resize(max);
        for (auto i = 0; i < max; ++i) {
            auto sum = 0;
            auto num = i;
            while (num) {
                sum += (num % 10);
                num /= 10;
            }
            m_sum[i] = sum;
        }
    }

    vector<int> m_sum; // 数位和
    int m_count = 0;
    vector<vector<bool>> m_mask; // 标记已经访问点
    int m_th = 0;
    int m_rows = 0;
    int m_cols = 0;
};

TEST(test, move) {
    Solution so;
    ASSERT_EQ(so.movingCount(0, 3, 3), 1);
    ASSERT_EQ(so.movingCount(3, 5, 5), 10);
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}