/**
 * 给定一个二叉树和其中的一个结点，请找出中序遍历顺序的下一个结点并且返回。注意，树中的结点不仅包含左右子结点，同时包含指向父结点的指针。
 */
#define CATCH_CONFIG_MAIN
#include <catch.hpp>

struct TreeLinkNode {
    int val;
    struct TreeLinkNode *left;
    struct TreeLinkNode *right;
    struct TreeLinkNode *next;
    TreeLinkNode(int x) :val(x), left(NULL), right(NULL), next(NULL) {

    }
};


class Solution {
public:
    TreeLinkNode* GetNext(TreeLinkNode* pNode)
    {
        if (!pNode) {
            return nullptr;
        }

        // find the left most of right tree
        if (pNode->right) {
            pNode = pNode->right;
            while (pNode->left) {
                pNode = pNode->left;
            }
            return pNode;
        }

        // find the first node in ancestor nodes, to which this node is in the left tree
        auto parent = pNode->next;
        while (parent) {
            if (parent->left == pNode) {
                return parent;
            }
            pNode = parent;
            parent = parent->next;
        }
        return nullptr;
    }
};

TEST_CASE("btree", "next node") {
    auto node1 = TreeLinkNode(1);
    auto node2 = TreeLinkNode(2);
    auto node3 = TreeLinkNode(3);
    auto node4 = TreeLinkNode(4);
    node1.left = &node2;
    node2.next = &node1;
    node2.left = &node3;
    node3.next = &node2;
    node2.right = &node4;
    node4.next = &node2;

    REQUIRE(Solution().GetNext(&node2) == &node4);
    REQUIRE(Solution().GetNext(&node4) == &node1);
    REQUIRE(Solution().GetNext(&node1) == nullptr);
}