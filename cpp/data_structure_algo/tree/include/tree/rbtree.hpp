#pragma once

#include "bstree.hpp"

namespace bstree {

template <typename Key, typename Val, typename Cmp = std::less<Key>,
    typename Alloc = std::allocator<std::pair<const Key, Val>>>
class RBMap {
public:
    using key_type = const Key;
    using map_type = Val;
    using key_compare = Cmp;
    using value_type = std::pair<key_type , map_type>;
    using allocator_type = Alloc;
    using size_type = std::size_t;

protected:
    struct ValCmp {
        explicit ValCmp(key_compare cmp): m_cmp(std::move(cmp)) {};

        bool operator()(const value_type &l, const value_type &r) const {
            return m_cmp(l.first, r.first);
        }
    protected:
        key_compare m_cmp;
    };

    enum class Color {
        BLACK = 0,
        RED,
    };

    using node_type = BSTAugNode<value_type, Color>;
    using node_allocator_type = typename allocator_type::template rebind<node_type>::other;

    static inline Color getColor(const node_type *node) {
        if (!node) {
            return Color::BLACK;
        }
        return node->aug;
    }

    /**
     * recolor or rotate
     * @param node
     */
    void fixUpInsert(node_type *node) {
        while(node) {
            auto parent = node->getParent();
            // reach root
            if (!parent) {
                node->aug = Color::BLACK;
                m_root = node;
                return;
            }

            // all done
            if (node->aug == Color::BLACK || parent->aug == Color::BLACK) {
                return;
            }

            // parent red
            auto grand_pa = parent->getParent(); // must not be null
            node_type *uncle;
            bool parent_left;
            if (grand_pa->getLeft() == parent) {
                parent_left = true;
                uncle = grand_pa->getRight();
            } else {
                parent_left = false;
                uncle = grand_pa->getLeft();
            }
            auto uncle_color = getColor(uncle);

            // CASE1: parent red, uncle red. push red to grand_pa
            if (uncle_color == Color::RED) {
                grand_pa->aug = Color::RED;
                parent->aug = Color::BLACK;
                uncle->aug = Color::BLACK;
                node = grand_pa;
                continue;
            }

            if (parent_left) {
                auto node_right = parent->getRight() == node;
                // CASE2: left-right; 1. rotate left at parent to left-left case
                if (node_right) {
                    LeftRotate(parent);
                    std::swap(parent, node);
                }
                // CASE3: left-left; 1. rotate right at grand_pa, 2. re-color
                RightRotate(grand_pa);
            } else {
                auto node_left = parent->getLeft() == node;
                // CASE4: right-left
                if (node_left) {
                    RightRotate(parent);
                    std::swap(parent, node);
                }
                // CASE5: right-right
                LeftRotate(grand_pa);
            }
            // now parent move to top, re-color it to black; grand_pa became child of parent, re-color to red
            parent->aug = Color::BLACK;
            grand_pa->aug = Color::RED;
            node = parent;
        }
    }

    void fixUpRemove(node_type* node, node_type* parent) {
        // only one node
        if (!parent) {
            m_root = nullptr;
            return;
        }

        // no need to fix red
        if (node->aug == Color::RED) {
            return;
        }

        
    }

    ValCmp m_cmp;       // node comparator
    node_type *m_root = nullptr;  // root node
    node_allocator_type m_alloc;    // node allocator
    size_type m_count = 0;
public:
    using iterator = BstIterator<value_type, node_type>;
    using const_iterator = BstConstIterator<value_type, node_type>;

    explicit RBMap(const key_compare& cmp = key_compare(), const allocator_type& alloc = allocator_type()):
    m_cmp(cmp), m_alloc(alloc) {}

    ~RBMap() {
        clear();
    }

    size_type size() const {
        return m_count;
    }

    bool empty() const {
        return m_count == 0;
    }

    void clear() {
        PostOrderTravRecur(m_root, [&](auto ptr){
            m_alloc.destroy(ptr);
            m_alloc.deallocate(ptr, 1);
            return true;
        });
        m_root = nullptr;
        m_count = 0;
    }

    std::pair<iterator, bool> insert(value_type&& kv) {
        node_type *prev;
        node_type *next;
        std::tie(prev, next) = BstSearchHelper(m_root, kv, m_cmp);
        node_type *node = nullptr;
        if (prev && !m_cmp(prev->val, kv)) {
            node = prev;
        } else if (next && m_cmp(next->val, kv)) {
            node = next;
        }
        // found
        if (node) {
            node->val.second = std::move(kv.second);
            return make_pair(iterator(node), false);
        }

        // allocate new node
        node = m_alloc.allocate(1);
        m_alloc.construct(node, std::move(kv));

        // put non-root node
        node->aug = Color::RED;
        if (prev) {
            prev->setRight(node);
            node->setParent(prev);
        } else if (next) {
            next->setLeft(node);
            node->setParent(next);
        }
        fixUpInsert(node);
        ++m_count;
        return std::make_pair(iterator(node), true);
    }

    iterator erase(iterator it) {
        auto node = *it;
        ++it;

        // node is not leaf, find successor and replace
        if (node->getLeft() && node->getRight()) {
            auto successor = findLeftMost(node->getRight());
            std::swap(node->val, successor->val);
            std::swap(node->aug, successor->aug);
            std::swap(node, successor);
        }

        // now node has at most one child
        auto parent = node->getParent();
        BstUnlinkNode(node);
        fixUpRemove(node, parent);
        m_alloc.destroy(node);
        m_alloc.deallocate(node, 1);
        --m_count;
        return it;
    }
};

}
