#include <vector>
#include "gtest/gtest.h"

using namespace std;

class Solution {
    vector<vector<bool>> rowUsed{};
    vector<vector<bool>> colUsed{};
    vector<vector<bool>> cubUsed{};
    vector<vector<char>> *m_board = nullptr;
public:
    void solveSudoku(vector<vector<char>>& board) {
        rowUsed = vector<vector<bool>>(9, vector<bool>(10, false));
        colUsed = vector<vector<bool>>(9, vector<bool>(10, false));
        cubUsed = vector<vector<bool>>(9, vector<bool>(10, false));
        m_board = &board;
        for (auto i = 0; i < 9; ++i) {
            for (auto j = 0; j < 9; ++j) {
                if (board[i][j] != '.') {
                    int num = board[i][j] - '0';
                    rowUsed[i][num] = true;
                    colUsed[j][num] = true;
                    cubUsed[cub(i, j)][num] = true;
                }
            }
        }
        dfs(0, 0);
    }

    int cub(int i, int j) {
        i /= 3;
        j /= 3;
        return i * 3 + j;
    }

    bool dfs(int i, int j) {
        auto &board = *m_board;
        while (i < 9 && board[i][j] != '.') {
            ++j;
            if (j == 9) {
                ++i;
                j = 0;
            }
        }
        if (i == 9) {
            return true;
        }
        for (int val = 1; val <= 9; ++val) {
            if (rowUsed[i][val] || colUsed[j][val] || cubUsed[cub(i, j)][val]) {
                continue;
            }
            rowUsed[i][val] = colUsed[j][val] = cubUsed[cub(i, j)][val] = true;
            board[i][j] = char('0' + val);
            if (dfs(i, j)) {
                return true;
            }
            board[i][j] = '.';
            rowUsed[i][val] = colUsed[j][val] = cubUsed[cub(i, j)][val] = false;
        }
        return false;
    }
};

TEST(test, soduku) {
    vector<vector<char>> input({{'5','3','.','.','7','.','.','.','.'},
                                {'6','.','.','1','9','5','.','.','.'},
                                {'.','9','8','.','.','.','.','6','.'},
                                {'8','.','.','.','6','.','.','.','3'},
                                {'4','.','.','8','.','3','.','.','1'},
                                {'7','.','.','.','2','.','.','.','6'},
                                {'.','6','.','.','.','.','2','8','.'},
                                {'.','.','.','4','1','9','.','.','5'},
                                {'.','.','.','.','8','.','.','7','9'}});
    Solution().solveSudoku(input);
    ASSERT_EQ(input, vector<vector<char>>(
            {{'5','3','4','6','7','8','9','1','2'},
             {'6','7','2','1','9','5','3','4','8'},
             {'1','9','8','3','4','2','5','6','7'},
             {'8','5','9','7','6','1','4','2','3'},
             {'4','2','6','8','5','3','7','9','1'},
             {'7','1','3','9','2','4','8','5','6'},
             {'9','6','1','5','3','7','2','8','4'},
             {'2','8','7','4','1','9','6','3','5'},
             {'3','4','5','2','8','6','1','7','9'}}));
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
