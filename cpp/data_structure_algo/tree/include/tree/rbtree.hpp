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
    void fixUp(node_type *node) {
        while(node) {
            auto parent = node->getParent();

            if (!parent) {
                node->aug = Color::BLACK;
                return;
            }

            // no need
            if (parent->aut == Color::BLACK) {
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


        }
    }

    ValCmp m_cmp;       // node comparator
    node_type *m_root = nullptr;  // root node
    node_allocator_type m_alloc;    // node allocator
public:
    using iterator = BstIterator<value_type, node_type>;
    using const_iterator = BstConstIterator<value_type, node_type>;

    explicit RBMap(const key_compare& cmp = key_compare(), const allocator_type& alloc = allocator_type()):
    m_cmp(cmp), m_alloc(alloc) {}

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

        // put root
        if (!m_root) {
            node->aug = Color::BLACK;
            m_root = node;
            return iterator(node);
        }

        // put non-root node
        node->aug = Color::RED;
        if (prev) {
            prev->setRight(node);
            node->setParent(prev);
        } else {
            next->setLeft(node);
            node->setParent(next);
        }
        fixUp(node);
        return std::make_pair(iterator(node), true);
    }
};

}
