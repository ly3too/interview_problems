/**
 * 题目描述
把一个数组最开始的若干个元素搬到数组的末尾，我们称之为数组的旋转。
输入一个非递减排序的数组的一个旋转，输出旋转数组的最小元素。
例如数组[3,4,5,1,2]为[1,2,3,4,5]的一个旋转，该数组的最小值为1。
NOTE：给出的所有元素都大于0，若数组大小为0，请返回0。
 */

#include <vector>
#include "gtest/gtest.h"

using namespace std;

class Solution {
public:
    int minNumberInRotateArray(const vector<int> &rotateArray) {
        //assert(!rotateArray.empty());
        return minNumRotate(rotateArray, 0, rotateArray.size() - 1);
    }

private:
    static int minNumRotate(const vector<int> &arr, size_t l, size_t r) {
        auto mid = (l + r) / 2;
        // 中间最大, 一定在右
        if (arr[l] < arr[mid] && arr[mid] > arr[r]) {
            return minNumRotate(arr, mid + 1, r);

        // 中间比右小,一定在左包括mid
        } else if (arr[mid] < arr[r]) {
            return minNumRotate(arr, l, mid);

        // 中间比左小， 与右等, 一定在左
        } else if (arr[l] < arr[mid]) {
            return minNumRotate(arr, l, mid);
        // 都相等
        } else {
            return minNum(arr, l, r);
        }
    }

    static int minNum(const vector<int> &arr, size_t l, size_t r) {
        int val = arr[l];
        for (auto i = l; i <= r; ++i) {
            if (arr[i] < val) {
                return arr[i];
            }
        }
        return val;
    }
};

TEST(test, rotate) {
    Solution so;
    ASSERT_EQ(so.minNumberInRotateArray({1,2,3,4,5}), 1);
    ASSERT_EQ(so.minNumberInRotateArray({2, 3, 4, 0, 1, 1}), 0);
    ASSERT_EQ(so.minNumberInRotateArray({1, 1, 1, 0, 1, 1}), 0);
    ASSERT_EQ(so.minNumberInRotateArray({1, 1, 1, 1, 0, 1}), 0);
    ASSERT_EQ(so.minNumberInRotateArray({1, 0, 1, 1, 1, 1}), 0);
    ASSERT_EQ(so.minNumberInRotateArray({6, 1, 2, 3, 4, 5}), 1);
}

int main () {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
