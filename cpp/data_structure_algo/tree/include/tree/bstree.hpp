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
struct BSTNode: public BSTBaseNode<BSTNode<T>> {
    using value_type = T;
    value_type val;

    template<typename ...Args>
    explicit BSTNode(Args&&... args): BSTBaseNode<BSTNode<T>>(), val(std::forward<Args>(args)...) {}
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
std::tuple<NT *, NT *> BstSearchHelper(NT *root, const T &val, const Cmp &cmp = Cmp()) {
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
NT *BstSearch(NT *root, const T &val, const Cmp &cmp = Cmp()) {
    using PN = NT *; // Pointer of node
    PN next_smaller = nullptr;
    PN next_larger = nullptr;

    std::tie(next_smaller, next_larger) = BstSearchHelper<T, NT, Cmp>(root, val, cmp);
    // smaller node is not smaller
    if (next_smaller && !cmp(next_smaller->val, val)) {
        return next_smaller;
    }
    // larger node is smaller or eq
    if (next_larger && cmp(next_larger->val, val)) {
        return next_larger;
    }
    return nullptr;
}

template <typename T, typename NT = BSTNode<T>, typename Cmp = std::less<T>>
NT *BstInsert(NT *root, NT *node, const Cmp &cmp = Cmp()) {
    using PN = NT *; // Pointer of node
    PN next_smaller = nullptr;
    PN next_larger = nullptr;

    std::tie(next_smaller, next_larger) = BstSearchHelper<T, NT, Cmp>(root, node->val, cmp);

    node->setLeft(nullptr);
    node->setRight(nullptr);
    if (next_smaller && next_smaller->getRight() == nullptr) {
        // insert to smaller node's right
        next_smaller->setRight(node);
        node->setParent(next_smaller);
        return node;
    } else if (next_larger && next_larger->getLeft() == nullptr) {
        // insert to larger node's left
        next_larger->setLeft(node);
        node->setParent(next_larger);
        return node;
    }

    // no node found
    node->setParent(nullptr);
    return node;
}

/**
 * find the left most node
 * @tparam NT
 * @param root root of tree, must not be null
 * @return
 */
template<typename NT>
inline NT *findLeftMost(NT *root) {
    while(root->getLeft()) {
        root = root->getLeft();
    }
    return root;
}

/**
 * find the right most node
 * @tparam NT
 * @param root root of tree, must not be null
 * @return
 */
template<typename NT>
inline NT *findRightMost(NT *root) {
    while(root->getRight()) {
        root = root->getRight();
    }
    return root;
}

/**
 * find the first parent to the right
 *
 * @tparam NT
 * @param root
 * @return
 */
template<typename NT>
inline NT *findFirstRightParent(NT *root) {
    while (root->getParent() && root->getParent()->getLeft() != root) {
        root = root->getParent();
    }
    return root->getParent();
}

/**
 * find the first parent to the left
 *
 * @tparam NT
 * @param root
 * @return
 */
template<typename NT>
inline NT *findFirstLeftParent(NT *root) {
    while (root->getParent() && root->getParent()->right != root) {
        root = root->getParent();
    }
    return root->getParent();
}

/**
 * unlink node from a tree, node must exist
 * @tparam T
 * @tparam NT
 * @param root
 * @param node
 * @return
 */
template <typename T, typename NT = BSTNode<T>>
NT *BstUnlinkNode(NT *root, NT *node) {
    if (!node) {
        return root;
    }

    auto const left = node->getLeft();
    auto right = node->getRight();
    auto const parent = node->getParent();
    // single child, move child up
    if (!left || !right) {
        auto next = left;
        if (right) {
            next = right;
        }
        // delete the root node
        if (!parent) {
            if (next) {
                next->setParent(nullptr);
            }
            return next;
        }

        if (parent->getLeft() == node) {
            parent->setLeft(next);
        } else {
            parent->setRight(next);
        }
        if (next)
            next->setParent(parent);
        return root;
    }

    // find the left most right child, which must has children count of less or equal to 1
    auto successor = findLeftMost(right);
    // unlink it from tree of right tree
    BstUnlinkNode<T, NT>(node, successor);
    // reset parent link
    successor->setParent(parent);
    if (!parent) {
        root = successor;
    } else {
        if (parent->getLeft() == node) {
            parent->setLeft(successor);
        } else {
            parent->setRight(successor);
        }
    }
    // swap the link
    successor->setLeft(left);
    left->setParent(successor);
    right = node->getRight();
    successor->setRight(right);
    if (right) {
        right->setParent(successor);
    }
    return root;
}

template <typename T, typename NT = BSTNode<T>>
class BstConstIterator {
public:
    using value_type = T;
    using node_type = NT;
protected:
    node_type *m_node = nullptr;
public:
    explicit BstConstIterator(node_type *node): m_node(node) {}
    BstConstIterator(const BstConstIterator &other) = default;
    BstConstIterator(BstConstIterator &&other) = default;

    const value_type &operator*() const {
        return m_node->val;
    }

    const value_type *operator->() const {
        return &m_node->val;
    }

    bool operator==(const BstConstIterator &other) const {
        return m_node == other.m_node;
    }

    bool operator!=(const BstConstIterator &other) const {
        return m_node != other.m_node;
    }

    BstConstIterator &operator++() {
        if (m_node->getRight()) {
            m_node = findLeftMost(m_node->getRight());
            return *this;
        }
        m_node = findFirstRightParent(m_node);
        return *this;
    }

    BstConstIterator &operator--() {
        if (m_node->getLeft()) {
            m_node = findRightMost(m_node->getLeft());
            return *this;
        }
        m_node = findFirstLeftParent(m_node);
        return *this;
    }

    BstConstIterator operator++(int) {
        auto ret = BstConstIterator(*this);
        ++(*this);
        return ret;
    }

    BstConstIterator operator--(int) {
        auto ret = BstConstIterator(*this);
        --(*this);
        return ret;
    }
};

template <typename T, typename NT = BSTNode<T>>
class BstIterator: public BstConstIterator<T, NT> {
public:
    using value_type = T;
    using node_type = NT;

protected:

public:
    explicit BstIterator(node_type *node): BstConstIterator<T, NT>(node) {}

    value_type &operator*() {
        return BstConstIterator<T, NT>::m_node->val;
    }

    value_type *operator->() {
        return &BstConstIterator<T, NT>::m_node->val;
    }
};



};
