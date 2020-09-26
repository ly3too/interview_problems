#include "tree/btree.hpp"
#include "gtest/gtest.h"

using namespace std;
using namespace btree;

template<typename T>
struct MirrNode: public BaseNode<MirrNode<T>> {
    T val;
    inline MirrNode *getLeft() {
        return this->right;
    }
    inline MirrNode *getRight() {
        return this->left;
    }
};

TEST(test, tree) {
    auto n1 = new Node<int>(1);
    auto n2 = new Node<int>(2);
    auto n3 = new Node<int>(3);
    auto n4 = new Node<int>(4);
    auto n5 = new Node<int>(5);
    auto n6 = new Node<int>(6);
    auto n7 = new Node<int>(7);
    n3->left = n2;
    n3->right = n5;
    n2->left = n1;
    n5->left = n4;
    n5->right = n6;
    n6->right = n7;

    // in order
    vector<int> out;
    auto func = [&out](Node<int> *ptr) {out.emplace_back(ptr->val); return true;};
    auto ret = InOrderTravRecur<Node<int>>(n3, func);
    ASSERT_EQ(ret, nullptr);
    ASSERT_EQ(out, vector<int>({1, 2, 3, 4, 5, 6, 7}));

    out.clear();
    ret = InOrderTravRecur<Node<int>>(n3, [&out](auto ptr){
        out.emplace_back(ptr->val); return ptr->val != 6;});
    ASSERT_EQ(ret, n6);
    ASSERT_EQ(out, vector<int>({1, 2, 3, 4, 5, 6}));

    out.clear();
    ret = InOrderTrav<Node<int>>(n3, func);
    ASSERT_EQ(ret, nullptr);
    ASSERT_EQ(out, vector<int>({1, 2, 3, 4, 5, 6, 7}));

    out.clear();
    ret = reinterpret_cast<Node<int> *>(InOrderTrav<MirrNode<int>>(
            reinterpret_cast<MirrNode<int> *>(n3), [&out](auto ptr){
                out.emplace_back(ptr->val);
                return true;
            }));
    ASSERT_EQ(ret, nullptr);
    ASSERT_EQ(out, vector<int>({7, 6, 5, 4, 3, 2, 1}));

    // pre order
    out.clear();
    ret = PreOrderTravRecur<Node<int>>(n3, func);
    ASSERT_EQ(ret, nullptr);
    ASSERT_EQ(out, vector<int>({3, 2, 1, 5, 4, 6, 7}));

    out.clear();
    ret = PreOrderTrav<Node<int>>(n3, func);
    ASSERT_EQ(ret, nullptr);
    ASSERT_EQ(out, vector<int>({3, 2, 1, 5, 4, 6, 7}));

    // post order
    out.clear();
    ret = PostOrderTravRecur<Node<int>>(n3, func);
    ASSERT_EQ(ret, nullptr);
    ASSERT_EQ(out, vector<int>({1, 2, 4, 7, 6, 5, 3}));

    out.clear();
    ret = PostOrderTrav<Node<int>>(n3, func);
    ASSERT_EQ(ret, nullptr);
    ASSERT_EQ(out, vector<int>({1, 2, 4, 7, 6, 5, 3}));
    PostOrderTravRecur(n3, [](auto ptr){delete ptr; return true;});
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
