//题目描述
// 输入某二叉树的前序遍历和中序遍历的结果，请重建出该二叉树。假设输入的前序遍历和中序遍历的结果中都不含重复的数字。
// 例如输入前序遍历序列{1,2,4,7,3,5,6,8}和中序遍历序列{4,7,2,1,5,3,8,6}，则重建二叉树并返回。

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <vector>
#include <functional>
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
        return reConsRecurse(pre, 0, pre.size(), 0);
    }

    static void inorderTrav(TreeNode *root, vector<int> &vin) {
        if (!root) {
            return;
        }
        inorderTrav(root->left, vin);
        vin.emplace_back(root->val);
        inorderTrav(root->right, vin);
    }

    static void postOrderTrav(TreeNode *root, const function<void(TreeNode *)> &func) {
        if (!root) {
            return;
        }
        if (root->left) {
            postOrderTrav(root->left, func);
        }
        if (root->right) {
            postOrderTrav(root->right, func);
        }
        func(root);
    }

private:
    /**
     * reconstruct tree recursively
     * @param pre pre order vals
     * @param vin in order vals
     * @param p_start start pos
     * @param p_end end pos
     * @param i_start start of in order
     * @return
     */
    TreeNode *reConsRecurse(const vector<int> &pre, int p_start, int p_end, int i_start) {
        if (p_start >= p_end) {
            return nullptr;
        }
        auto root_val = pre[p_start];
        auto root = new TreeNode(root_val);
        auto root_vin_idx = m_in_idx_map[root_val]; // root node index at in order array
        auto left_len = root_vin_idx - i_start; // left subtree len

        root->left = reConsRecurse(pre, p_start + 1, p_start + left_len + 1, i_start);
        root->right = reConsRecurse(pre, p_start + left_len + 1, p_end, root_vin_idx + 1);

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
    Solution::postOrderTrav(ret, [](auto ptr){delete ptr;});
}