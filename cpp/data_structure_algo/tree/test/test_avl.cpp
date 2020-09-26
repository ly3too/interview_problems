#include "gtest/gtest.h"
#include "tree/avl.hpp"

using namespace std;
using namespace bstree;

template<typename Key, typename Val, typename Cmp = std::less<Key>,
        typename Alloc = std::allocator<std::pair<Key, Val>>>
struct TestAVL: public AVLMap<Key, Val, Cmp, Alloc> {
    bool checkTree() {
        auto ret = true;
        PostOrderTrav(this->m_root, [&](auto node) {
            auto exp = std::max(this->getHight(node->getLeft()), this->getHight(node->getRight())) + 1;
            if (exp != node->aug) {
                ret = false;
            }
            if (node->getLeft() && node->getLeft()->getParent() != node) {
                ret = false;
            }
            if (node->getRight() && node->getRight()->getParent() != node) {
                ret = false;
            }
            return true;
        });
        return ret;
    }

    void print() {
        PrintTree(cout, this->m_root, [](auto ptr){
            return to_string((ptr->val).first) + "(" + to_string(ptr->aug) + ")";
        });
    }
};

TEST(test, avl) {
    // test insert
    auto tree = TestAVL<int, int>();
    for (auto i = 0; i < 100; ++i) {
        tree.insert(make_pair(i, i));
    }
    for (auto i = 50; i < 100; ++i) {
        tree.insert(make_pair(i, i));
    }
    ASSERT_TRUE(tree.checkTree());

    // check item
    auto idx = 0;
    for (auto &item: tree) {
        ASSERT_EQ(item.first, idx);
        ++idx;
    }

    // test find and erase
    for (auto i = 50; i < 70; ++i) {
        //test find
        auto it = tree.find({i, i});
        ASSERT_EQ(it->first, i);
        it = tree.erase(it);
        ASSERT_TRUE(tree.checkTree());
        ASSERT_EQ(it->second, i + 1);
        ASSERT_EQ(tree.size(), 100 - (i - 49));
    }

    tree.print();
}


