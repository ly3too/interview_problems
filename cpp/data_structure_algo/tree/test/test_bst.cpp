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
