#include "gtest/gtest.h"
#include "tree/avl.hpp"
#include "tree/rbtree.hpp"
#include <chrono>

using namespace std;
using namespace bstree;
using namespace std::chrono;

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
        auto it = tree.find(i);
        ASSERT_EQ(it->first, i);
        it = tree.erase(it);
        ASSERT_TRUE(tree.checkTree());
        ASSERT_EQ(it->second, i + 1);
        ASSERT_EQ(tree.size(), 100 - (i - 49));
    }

    tree.print();
}

template<typename Key, typename Val, typename Cmp = std::less<Key>,
    typename Alloc = std::allocator<std::pair<Key, Val>>>
struct TestRB: public RBMap<Key, Val, Cmp, Alloc> {
    using Color = typename RBMap<Key, Val, Cmp, Alloc>::Color;
    using node_type = typename RBMap<Key, Val, Cmp, Alloc>::node_type;

#if (WIN32)
    const string red = "";
    const string reset = "";
#else
    const string red = "\033[0;31m";
    const string reset = "\033[0m";
#endif

    bool checkTree() {
        auto ret = true;
        PostOrderTrav(this->m_root, [&](auto node) {
            // check color: no adjacent red
            if (node->getLeft() && node->getLeft()->aug == Color::RED && node->aug == Color::RED) {
                ret = false;
            }
            if (node->getRight() && node->getRight()->aug == Color::RED && node->aug == Color::RED) {
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

        // check black depth
        cout << "black depth: " << checkBlackDepth(this->m_root) << endl;
        return ret;
    }

    int checkBlackDepth(node_type* root) {
        if (!root) {
            return 0;
        }

        auto left = checkBlackDepth(root->getLeft());
        auto right = checkBlackDepth(root->getRight());

        if (left != right) {
            throw runtime_error("depth invalid");
        }

        auto cur = root->aug == Color::BLACK ? 1 : 0;
        return cur + left;
    }

    void print() {
        PrintTree(cout, this->m_root, [&](auto ptr){
            if (ptr->aug == Color::BLACK) {
                return to_string((ptr->val).first) + "(*)";
            }
            return red + to_string((ptr->val).first) + reset;
        });
    }
};

TEST(test, rb) {
    auto rbt = TestRB<int, int>();
    for (auto i = 0; i < 100; ++i) {
        rbt.insert(make_pair(i, i));
        ASSERT_EQ(rbt.size(), i + 1);
    }
    ASSERT_TRUE(rbt.checkTree());
    rbt.print();

    auto it = rbt.begin();
    for (int i = 0; i < 100; ++i) {
        ASSERT_EQ(it->first, i);
        ++it;
    }
    for (int i = 0; i < 100; ++i) {
        auto ret = rbt.erase(rbt.find(i));
        ASSERT_EQ(rbt.size(), 100 - i - 1);
        ASSERT_TRUE(rbt.checkTree());
        if (i != 99) {
            ASSERT_EQ(ret->second, i + 1);
        } else {
            ASSERT_EQ(ret, rbt.end());
        }
    }
}

template<typename Map>
void testMap(Map &t, int cnt) {
    auto cnt2 = cnt;
    while(cnt2--) {
        t.insert(make_pair(cnt2, cnt2));
    }
    cnt2 = cnt;
    while(cnt2--) {
        t.erase(t.find(cnt2));
    }
}

TEST(test, perf) {

    auto total_run = 100;
    auto elem_cnt = 1000;

    auto t1 = steady_clock::now();
    for (int i = 0; i < total_run; ++i) {
        auto t = map<int, int>();
        testMap(t, elem_cnt);
    }
    auto t2 = steady_clock::now();
    std::chrono::duration<double> diff = t2 - t1;
    cout << "map: " << diff.count() << endl;

    auto t3 = steady_clock::now();
    for (int i = 0; i < total_run; ++i) {
        auto t = AVLMap<int, int>();
        testMap(t, elem_cnt);
    }
    auto t4 = steady_clock::now();
    std::chrono::duration<double> diff2 = t4 - t3;
    cout << "avl map: " << diff2.count() << endl;
}


