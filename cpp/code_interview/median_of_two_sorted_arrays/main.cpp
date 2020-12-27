// https://leetcode-cn.com/problems/median-of-two-sorted-arrays/

#include <vector>
#include <algorithm>

#include "gtest/gtest.h"

using namespace std;

class Solution {
    vector<int> *m_nums1 = nullptr;
    vector<int> *m_nums2 = nullptr;
public:
    double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) {
        m_nums1 = &nums1;
        m_nums2 = &nums2;
        auto m = nums1.size();
        auto n = nums2.size();
        auto len = m + n;
        if (len % 2 == 0) {
            return (getKth((len - 1)/2) + getKth((len - 1)/2 + 1)) / 2;
        } else {
            return getKth((len - 1)/2);
        }
    }

    double getKth(int k) {
        auto s1 = 0;
        auto s2 = 0;
        auto& nums1 = *m_nums1;
        auto& nums2 = *m_nums2;
        while (true) {
            if (s1 >= nums1.size()) {
                return nums2[s2 + k];
            }
            if (s2 >= nums2.size()) {
                return nums1[s1 + k];
            }
            if (k == 0) {
                return min(nums1[s1], nums2[s2]);
            }

            auto mid = (k - 1) / 2;
            auto p1 = min(int(nums1.size()) - 1, s1 + mid);
            auto p2 = min(int(nums2.size()) - 1, s2 + mid);
            if (nums1[p1] < nums2[p2]) {
                // 排除 mid + 1 个 p1
                k -= (p1 - s1 + 1);
                s1 = p1 + 1;
            } else {
                // 排除 mid + 1个 p2
                k -= (p2 - s2 + 1);
                s2 = p2 + 1;
            }
        }
    }
};

TEST(test, mid) {
    {
        vector<int> v1 = {1,3};
        vector<int> v2 = {2};
        auto ret = Solution().findMedianSortedArrays(v1, v2);
        ASSERT_FLOAT_EQ(ret, 2.0);
    }
    {
        vector<int> v1 = {1};
        vector<int> v2 = {};
        auto ret = Solution().findMedianSortedArrays(v1, v2);
        ASSERT_FLOAT_EQ(ret, 1.0);
    }
    {
        vector<int> v1 = {1, 2, 3, 4};
        vector<int> v2 = {1, 2, 3, 4};
        auto ret = Solution().findMedianSortedArrays(v1, v2);
        ASSERT_FLOAT_EQ(ret, 2.5);
    }
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
