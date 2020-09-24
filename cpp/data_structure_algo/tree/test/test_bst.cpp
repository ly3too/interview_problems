#include "tree/bstree.hpp"
#include "gtest/gtest.h"
#include <map>
#include <random>

using namespace std;
using namespace bstree;


TEST(test, bst_search) {
    mt19937 gen{random_device()()};
    uniform_int_distribution<int> dist(1, 100);
    auto vec = vector<int>();
    auto val1 = dist(gen);
    vec.emplace_back(val1);
    auto root = new BSTNode<int>(val1);

    for (auto i = 0; i < 20; ++i) {
        auto val = dist(gen);
        auto node = new BSTNode<int>(val);
        vec.emplace_back(val);
        BstInsert<int>(root, node);
    }

    auto vec2 = vector<int>();
    InOrderTrav(root, [&vec2](auto node){
        vec2.emplace_back(node->val);
        return true;
    });
    sort(vec.begin(), vec.end());
    ASSERT_EQ(vec, vec2);
}
