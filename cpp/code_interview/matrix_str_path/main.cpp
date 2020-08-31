/**
 * 题目描述
请设计一个函数，用来判断在一个矩阵中是否存在一条包含某字符串所有字符的路径。路径可以从矩阵中的任意一个格子开始，每一步可以在矩阵中
 向左，向右，向上，向下移动一个格子。如果一条路径经过了矩阵中的某一个格子，则该路径不能再进入该格子。
 例如 \begin{bmatrix} a & b & c &e \\ s & f & c & s \\ a & d & e& e\\ \end{bmatrix}\quad
   矩阵中包含一条字符串"bcced"的路径，但是矩阵中不包含"abcb"路径，因为字符串的第一个字符b占据了矩阵中的第一行第二个格子之后，路径不能再次进入该格子。
 */

#include "gtest/gtest.h"

class Solution {
public:
    bool hasPath(const char* matrix, int rows, int cols, const char* str) {
        this->matrix = matrix;
        this->rows = rows;
        this->cols = cols;
        auto cnt = rows * cols;
        marked = new char[cnt]();

        auto ret = false;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                ret =  hasPath(r, c, str);
                if (ret) {
                    break;
                }
            }
            if (ret) {
                break;
            }
        }

        delete []marked;
        return ret;
    }

private:
    /**
     * 递归查找
     * @param r 行
     * @param c 列
     * @param str 剩余字符
     * @return
     */
    bool hasPath(int r, int c, const char *str) {
        if (*str == '\0') {
            return true;
        }

        // 超限
        if (r >= rows || r < 0 || c >= cols || c < 0) {
            return false;
        }
        // 当前未匹配
        auto cur = cols * r + c;
        if (marked[cur] || *str != matrix[cur]) {
            return false;
        }

        // 当前匹配
        marked[cur] = '\1';
        auto next = str + 1;
        if (hasPath(r + 1, c, next) || hasPath(r - 1, c, next)
            || hasPath(r, c + 1, next) || hasPath(r, c - 1, next)) {
            return true;
        }
        marked[cur] = '\0';
        return false;
    }

    const char *matrix = nullptr;
    char *marked = nullptr;
    int rows = 0;
    int cols = 0;
};

TEST(test, path) {
    auto matrix = "abcesfcsadee";
    Solution so;
    ASSERT_EQ(so.hasPath(matrix, 3, 4, "bcced"), true);
    ASSERT_EQ(so.hasPath(matrix, 3, 4, "bccees"), true);
    ASSERT_EQ(so.hasPath(matrix, 3, 4, "bccda"), false);
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}