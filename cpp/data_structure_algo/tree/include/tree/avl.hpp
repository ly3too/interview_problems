#pragma once

#include "bstree.hpp"

namespace bstree {

template<typename Key, typename Val, typename Cmp = std::less<Key>,
    typename Alloc = std::allocator<std::pair<Key, Val>>>
class AVLMap {
public:
    using key_type = Key;
    using mapped_type = Val;
    using value_type = std::pair<key_type, mapped_type>;
    using size_type = std::size_t;
    using allocator_type = Alloc;
    using key_compare = Cmp;

protected:
    using node_type = BSTAugNode<value_type, int>;
    using node_alloc_type = typename Alloc::template rebind<node_type>::other;

    class PairKeyCmp {
    protected:
        key_compare m_cmp;
    public:
        PairKeyCmp(const key_compare &cmp): m_cmp(cmp) {}

        bool operator()(const value_type &l, const value_type &r) const {
            return m_cmp(l.first, r.first);
        }
    };

    node_type *m_root = nullptr;
    node_alloc_type m_alloc = node_alloc_type();
    PairKeyCmp m_cmp;
    size_type m_count = 0;

    inline static int getHight(node_type *node) {
        if (node) {
            return node->aug;
        } else {
            return 0;
        }
    }

    inline static void updateNodeHight(node_type *node) {
        node->aug = std::max(getHight(node->getLeft()), getHight(node->getRight())) + 1;
    }

    inline void leftRotateUpdate(node_type *node) {
        LeftRotate(node);
        updateNodeHight(node);
        updateNodeHight(node->getParent());
        if (node == m_root) {
            m_root = node->getParent();
        }
    }

    inline void rightRorateUpdate(node_type *node) {
        RightRotate(node);
        updateNodeHight(node);
        updateNodeHight(node->getParent());
        if (node == m_root) {
            m_root = node->getParent();
        }
    }

    void fixUp(node_type *node, bool once = true) {
        while (node) {
            auto left = node->getLeft();
            auto right = node->getRight();
            auto parent = node->getParent();
            auto left_hight = getHight(left);
            auto right_hight = getHight(right);
            node->aug = std::max(left_hight, right_hight) + 1;
            auto diff = left_hight - right_hight;
            if (diff <= 1 && diff >= -1) {
                node = parent;
                continue;
            }

            // found the first unbalanced node, rotate, then we are done
            if (diff > 1) {
                // left-right case
                if (getHight(left->getLeft()) < getHight(left->getRight())) {
                    // node became child of left
                    leftRotateUpdate(left);
                }
                // left-left case
                rightRorateUpdate(node);
                if (once) {
                    break;
                } else {
                    node = parent;
                    continue;
                }
            }
            if (getHight(right->getRight()) < getHight(right->getLeft())) {
                // right-left case
                rightRorateUpdate(right);
            }
            // right-right case
            leftRotateUpdate(node);
            if (once) {
                break;
            } else {
                node = parent;
                continue;
            }
        }
    }

public:
    using iterator = BstIterator<value_type, node_type>;
    using const_iterator = BstConstIterator<value_type, node_type>;

    AVLMap(const key_compare &cmp = key_compare(), const allocator_type &alloc = allocator_type()):
    m_cmp(cmp), m_alloc(alloc) {}

    void clear() {
        PostOrderTrav(m_root, [&](auto ptr){
            m_alloc.destroy(ptr);
            m_alloc.deallocate(ptr, 1);
            return true;
        });
        m_root = nullptr;
        m_count = 0;
    }

    ~AVLMap() {
        clear();
    }

    size_type size() const {
        return m_count;
    }

    // warning return no const iterator for simplicity
    iterator find(const key_type &key) const {
        value_type val{key, {}};
        auto node = BstSearch(m_root, val, m_cmp);
        return iterator(node);
    }

    iterator begin() const {
        return iterator(findLeftMost(m_root));
    }

    iterator end() const {
        return iterator(nullptr);
    }

    iterator rbegin() const {
        return iterator(findRightMost(m_root));
    }

    iterator rend() const {
        return iterator(nullptr);
    }

    template<typename Arg>
    std::pair<iterator, bool> insert(Arg &&arg) {
        node_type *prev;
        node_type *next;
        std::tie(prev, next) = BstSearchHelper(m_root, arg, m_cmp);
        node_type *node = nullptr;
        if (prev && !m_cmp(prev->val, arg)) {
            node = prev;
        } else if (next && m_cmp(next->val, arg)) {
            node = next;
        }

        // not found
        if (!node) {
            node = m_alloc.allocate(1);
            m_alloc.construct(node, std::forward<Arg>(arg));
            if (!m_root) {
                m_root = node;
            } else {
                if (prev) {
                    prev->setRight(node);
                    node->setParent(prev);
                } else if (next) {
                    next->setLeft(node);
                    node->setParent(next);
                }
            }

            ++m_count;
            fixUp(node);
            return std::make_pair(iterator(node), true);
        }

        // found
        if (std::is_rvalue_reference<Arg>::value) {
            node->val.second = std::move(arg.second);
        } else {
            node->val.second = arg.second;
        }
        return std::make_pair(iterator(node), false);
    }

    iterator erase(iterator pos) {
        auto node = pos.getNode();

        // node is not leaf, find successor and replace
        if (node->getLeft() && node->getRight()) {
            auto successor = findLeftMost(node->getRight());
            std::swap(node->val, successor->val);
            std::swap(node, successor);
        } else {
            ++pos;
        }

        auto parent = node->getParent();
        auto ret = BstUnlinkNode(node);
        fixUp(parent, false);
        if (m_root == node) {
            m_root = ret;
        }
        m_alloc.destroy(node);
        m_alloc.deallocate(node, 1);
        --m_count;
        return pos;
    }
};

}
