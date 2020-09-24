#pragma once
#include "btree.hpp"
#include <tuple>

namespace bstree {
using namespace btree;

template<typename NT>
struct BSTBaseNode: public BaseNode<NT> {
    inline NT *getParent() {
        return parent;
    }
    inline void setParent(NT *p) {
        parent = p;
    }

    NT *parent = nullptr;
};

template<typename T>
struct BSTNode: public BaseNode<BSTNode<T>> {
    using value_type = T;
    value_type val;

    template<typename ...Args>
    explicit BSTNode(Args&&... args): BaseNode<BSTNode<T>>(), val(std::forward<Args>(args)...) {}
};

// augmented tree node, red-back tree, avl tree
template<typename T, typename AT>
struct BSTAugNode: public BaseNode<BSTAugNode<T, AT>> {
    using value_type = T;
    using augment_type = AT;
    value_type val;
    augment_type aug = augment_type();

    template<typename ...Args>
    explicit BSTAugNode(Args&&... args): BaseNode<BSTAugNode<T, AT>>(), val(std::forward<Args>(args)...) {}

    template<typename ...Args>
    explicit BSTAugNode(const augment_type &a, Args&&... args):
    BaseNode<BSTAugNode<T, AT>>(), val(std::forward<Args>(args)...), aug(a) {}
};

/**
 * search the bst for val, return the largest smaller and smallest larger node.
 * if cmp is less, the larger node is exactly larger and smaller node is smaller node is smaller or equal;
 * if cmp is less or equal, the smaller node is exactly smaller, and larger node is larger or equal to val;
 *
 * @tparam T
 * @tparam NT
 * @tparam Cmp
 * @param root
 * @param val
 * @param cmp
 * @return
 */
template <typename T, typename NT = BSTNode<T>, typename Cmp = std::less<T>>
std::tuple<NT *, NT *> BstSearch(NT *root, const T &val, const Cmp &cmp = Cmp()) {
    using PN = NT *; // Pointer of node
    PN next_larger = nullptr;
    PN next_smaller = nullptr;
    while(root) {
        if (cmp(val, root->val)) {
            //val smaller or equal, goto left
            next_larger = root;
            root = root->getLeft();
        } else {
            //val larger,  goto right
            next_smaller = root;
            root = root->getRight();
        }
    }
    return std::make_tuple(next_smaller, next_larger);
}

template <typename T, typename NT = BSTNode<T>, typename Cmp = std::less<T>>
NT *BstInsert(NT *root, NT *node, const Cmp &cmp = Cmp()) {
    using PN = NT *; // Pointer of node
    PN next_smaller = nullptr;
    PN next_larger = nullptr;

    std::tie(next_smaller, next_larger) = BstSearch(node);

    node->setLeft(nullptr);
    node->setRight(nullptr);
    if (next_smaller && next_smaller->getRight == nullptr) {
        // insert to smaller node's right
        next_smaller->setRight(node);
        node->setParent(next_smaller);
        return node;
    } else if (next_larger && next_larger->getLeft() == nullptr) {
        // insert to larger node's left
        next_larger->setLeft(node);
        node->setParent(node);
        return node;
    }

    // no node found
    node->setParent(nullptr);
    return node;
}

}
