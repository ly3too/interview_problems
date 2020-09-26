#include "tree/bstree.hpp"
#include "gtest/gtest.h"
#include <map>
#include <random>
#include <algorithm>

using namespace std;
using namespace bstree;


TEST(test, bst_search) {
    mt19937 gen{random_device()()};
    uniform_int_distribution<int> dist(1, 100);
    auto vec = vector<int>();
    auto val1 = dist(gen);
    vec.emplace_back(val1);
    auto root = new BSTNode<int>(val1);

    for (auto i = 0; i < 100; ++i) {
        auto val = dist(gen);
        auto node = new BSTNode<int>(val);
        vec.emplace_back(val);
        BstInsert<int>(root, node);
    }
    PrintTree(cout, root);

    // should be sorted
    auto vec2 = vector<int>();
    InOrderTrav(root, [=, &vec2](auto node){
        vec2.emplace_back(node->val);
        EXPECT_TRUE((node->getParent() == nullptr && node == root)
            || node->getParent()->getLeft() == node
            || node->getParent()->getRight() == node);
        return true;
    });
    sort(vec.begin(), vec.end());
    ASSERT_EQ(vec, vec2);

    // test the iterator
    auto it1 = BstIterator<int>(findLeftMost(root));
    for (auto &val: vec) {
        ASSERT_EQ(val, *it1);
        ++it1;
    }
    auto it2 = BstIterator<int>(findRightMost(root));
    reverse(vec.begin(), vec.end());
    for (auto &val: vec) {
        ASSERT_EQ(val, *it2);
        it2--;
    }

    // insert two same node, should find the latter with std::less
    BstInsert<int>(root, new BSTNode<int>(110));
    auto n1 = new BSTNode<int>(110);
    BstInsert<int>(root, n1);
    ASSERT_EQ(BstSearch(root, 110), n1);
    ASSERT_EQ(BstSearch(root, 109), nullptr);

    // clean up
    PostOrderTravRecur(root, [](auto node) {delete node; return true;});
}

template <typename NT>
bool checkNodes(NT *root) {
    bool ok = true;
    PostOrderTravRecur<NT>(root, [&ok](auto ptr){
        if (ptr->getLeft() && ptr->getLeft()->getParent() != ptr) {
            ok = false;
            return false;
        }
        if (ptr->getRight() && ptr->getRight()->getParent() != ptr) {
            ok = false;
            return false;
        }
        return true;
    });
    return ok;
}

TEST(test, unlink) {
    auto n1 = BSTNode<int>(1);
    auto n2 = BSTNode<int>(2);
    auto n3 = BSTNode<int>(3);
    auto n4 = BSTNode<int>(4);
    auto n5 = BSTNode<int>(5);
    auto n6 = BSTNode<int>(6);
    n2.setLeft(&n1);
    n1.setParent(&n2);
    n2.setRight(&n5);
    n5.setParent(&n2);
    n5.setLeft(&n3);
    n3.setParent(&n5);
    n3.setRight(&n4);
    n4.setParent(&n3);
    n5.setRight(&n6);
    n6.setParent(&n5);

    ASSERT_EQ(checkNodes(&n2), true);

    auto root = BstUnlinkNode(&n2);
    ASSERT_EQ(checkNodes(root), true);
    ASSERT_EQ(root, &n3);
    ASSERT_EQ(root->getRight(), &n5);
    ASSERT_EQ(n5.getLeft(), &n4);

    auto ret = BstUnlinkNode(&n5);
    ASSERT_EQ(checkNodes(root), true);
    ASSERT_EQ(ret, &n6);
    ASSERT_EQ(root->getRight(), &n6);
    ASSERT_EQ(n6.getRight(), nullptr);
    ASSERT_EQ(n6.getLeft(), &n4);
}

TEST(test, rotate) {

    auto n1 = BSTAugNode<int, int>(1);
    auto n2 = BSTAugNode<int, int>(2);
    auto n3 = BSTAugNode<int, int>(3);
    auto n4 = BSTAugNode<int, int>(4);
    auto n5 = BSTAugNode<int, int>(5);

    n2.setLeft(&n1);
    n1.setParent(&n2);
    n2.setRight(&n3);
    n3.setParent(&n2);
    n3.setLeft(&n4);
    n4.setParent(&n3);
    n3.setRight(&n5);
    n5.setParent(&n3);

    auto root = &n2;
    ASSERT_TRUE(checkNodes(root));

    // left rotate
    auto ret = LeftRotate(&n3);
    ASSERT_EQ(ret, &n5);
    ASSERT_EQ(ret->getLeft(), &n3);
    ASSERT_EQ(ret->getRight(), nullptr);
    ASSERT_EQ(ret->getLeft()->getRight(), nullptr);
    ASSERT_TRUE(checkNodes(root));

    // right rotate back
    ret = RightRotate(ret);
    ASSERT_EQ(ret, &n3);
    ASSERT_EQ(ret->getRight(), &n5);
    ASSERT_EQ(ret->getLeft(), &n4);
    ASSERT_TRUE(checkNodes(root));

    // left rotate at root
    root = LeftRotate(root);
    ASSERT_EQ(root, &n3);
    ASSERT_EQ(root->getLeft(), &n2);
    ASSERT_EQ(root->getRight(), &n5);
    ASSERT_TRUE(checkNodes(root));
}