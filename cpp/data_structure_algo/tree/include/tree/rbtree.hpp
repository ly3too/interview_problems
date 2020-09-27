#pragma once

#include "bstree.hpp"

namespace bstree {

template <typename Key, typename Val, typename Cmp = std::less<Key>,
    typename Alloc = std::allocator<std::pair<const Key, Val>>>
class RBMap {
public:
    using key_type = Key;
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

    inline void leftRotateUpdateRoot(node_type *node) {
        LeftRotate(node);
        if (node == m_root) {
            m_root = node->getParent();
        }
    }

    inline void rightRotateUpdateRoot(node_type *node) {
        RightRotate(node);
        if (node == m_root) {
            m_root = node->getParent();
        }
    }

    void fixDoubleBlack(node_type *node, node_type* parent) {
        while(true) {
            // reached root
            if (node == m_root) {
                break;
            }
            auto is_left = parent->getLeft() == node; // must have sibling
            auto sib = is_left ? parent->getRight() : parent->getLeft();

            // CASE1: sibling is red, rotate and change it to black case
            if (sib->aug == Color::RED) {
                if (is_left) {
                    leftRotateUpdateRoot(parent);
                } else {
                    rightRotateUpdateRoot(parent);
                }
                parent->aug = Color::RED;
                sib->aug = Color::BLACK;
                continue;
            }

            // sib is now black
            auto sib_l_color = getColor(sib->getLeft());
            auto sib_r_color = getColor(sib->getRight());
            // CASE2: parent red, sib black, nephews black, switch color of parent and sib, done
            if (parent->aug == Color::RED && sib_l_color == Color::BLACK && sib_r_color == Color::BLACK) {
                parent->aug = Color::BLACK;
                sib->aug = Color::RED;
                break;
            }

            // now parent black, sib black
            // CASE3: parent, sib, sib's children all black; change sib's color to red, move cur node to parent, continue
            if (sib_l_color == Color::BLACK && sib_r_color == Color::BLACK) {
                sib->aug = Color::RED;
                node = parent;
                parent = node->getParent();
                continue;
            }

            // now at lest one nephew is red
            // CASE4: nephew is right-left case, change to right-right case
            if (is_left && sib_l_color == Color::RED) {
                auto sib_l = sib->getLeft();
                rightRotateUpdateRoot(sib);
                sib->aug = Color::RED;
                sib_l->aug = Color::BLACK;
                sib = sib_l;
            // CASE5: nephew is left-right case change to left-left
            } else if (!is_left && sib_r_color == Color::RED) {
                auto sib_r = sib->getRight();
                leftRotateUpdateRoot(sib);
                sib->aug = Color::RED;
                sib_r->aug = Color::BLACK;
                sib = sib_r;
            }

            // CASE6: nephew is right-right case,
            if (is_left) {
                auto sib_r = sib->getRight();
                leftRotateUpdateRoot(parent);
                std::swap(parent->aug, sib->aug);
                sib_r->aug = Color::BLACK;
                break;
            }
            // CASE7: nephew is left-left case
            auto sib_l = sib->getLeft();
            rightRotateUpdateRoot(parent);
            std::swap(parent->aug, sib->aug);
            sib_l->aug = Color::BLACK;
            break;
        }

        m_root->aug = Color::BLACK;
    }

    void fixUpRemove(node_type* node, node_type* parent, node_type* next) {
        // removed root
        if (!parent) {
            m_root = next;
            return;
        }

        // CASE1: removed red, no need to fix red
        if (node->aug == Color::RED) {
            return;
        }

        // CASE2: next node is RED, change it to black
        if (next && next->aug == Color::RED) {
            next->aug = Color::BLACK;
            return;
        }

        // double-black cases
        fixDoubleBlack(nullptr, parent);
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
        auto node = it.getNode();

        // node is not leaf, find successor and replace
        if (node->getLeft() && node->getRight()) {
            auto successor = findLeftMost(node->getRight());
            std::swap(node->val, successor->val);
            std::swap(node, successor);
        } else {
            ++it;
        }

        // now node has at most one child
        auto parent = node->getParent();
        auto next = BstUnlinkNode(node);
        fixUpRemove(node, parent, next);
        m_alloc.destroy(node);
        m_alloc.deallocate(node, 1);
        --m_count;
        return it;
    }

    iterator begin() const {
        return iterator(findLeftMost(m_root));
    }

    iterator end() const {
        return iterator(nullptr);
    }

    // need a reverse iterator
//    iterator rbegin() const {
//        return iterator(findRightMost(m_root));
//    }
//
//    iterator rend() const {
//        return iterator(nullptr);
//    }

    iterator find(const key_type &key) const {
        auto node = BstSearch(m_root, value_type(key, map_type()), m_cmp);
        return iterator(node);
    }
};

}
