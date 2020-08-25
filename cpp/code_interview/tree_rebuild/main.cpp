//题目描述
// 输入某二叉树的前序遍历和中序遍历的结果，请重建出该二叉树。假设输入的前序遍历和中序遍历的结果中都不含重复的数字。
// 例如输入前序遍历序列{1,2,4,7,3,5,6,8}和中序遍历序列{4,7,2,1,5,3,8,6}，则重建二叉树并返回。

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <vector>
#include <unordered_map>

using namespace std;

// Definition for binary tree
struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

class Solution {
public:
    TreeNode* reConstructBinaryTree(const vector<int> &pre, const vector<int> &vin) {
        // build val to index map for vin
        m_in_idx_map.clear();
        m_in_idx_map.reserve(pre.size());
        int idx = 0;
        for (auto val: vin) {
            m_in_idx_map[val] = idx;
            ++idx;
        }
        return reConsRecurse(pre, 0, pre.size());
    }

    static void inorderTrav(TreeNode *root, vector<int> &vin) {
        if (!root) {
            return;
        }
        inorderTrav(root->left, vin);
        vin.emplace_back(root->val);
        inorderTrav(root->right, vin);
    }

private:
    /**
     * reconstruct tree recursively
     * @param pre pre order vals
     * @param vin in order vals
     * @param start start pos
     * @param end end pos
     * @return
     */
    TreeNode *reConsRecurse(const vector<int> &pre, int start, int end) {
        if (start >= end) {
            return nullptr;
        }
        auto root_val = pre[start];
        auto root = new TreeNode(root_val);
        auto root_vin_idx = m_in_idx_map[root_val];

        root->left = reConsRecurse(pre, start + 1, root_vin_idx);
        root->right = reConsRecurse(pre, root_vin_idx + 1, end);

        return root;
    }

    unordered_map<int, int> m_in_idx_map;
};

TEST_CASE("case", "1") {
    vector<int> pre = {1,2,4,7,3,5,6,8};
    vector<int> vin = {4,7,2,1,5,3,8,6};
    auto ret = Solution().reConstructBinaryTree(pre, vin);

    REQUIRE(ret != nullptr);
    vector<int> out_in;
    Solution::inorderTrav(ret, out_in);
    REQUIRE(out_in == vin);
}